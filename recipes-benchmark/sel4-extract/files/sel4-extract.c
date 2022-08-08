#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define PAGE_SIZE 4096

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
#define BIT(x) (1 << (x))

#if 1
#define pr_dbg(...) do {printf("[DBG]: " __VA_ARGS__); printf("\t[%d@%s]\n", __LINE__, __func__); fflush(stdout);} while(0)
#else
#define pr_dbg(...)
#endif

typedef enum {
    Entry_Interrupt,
    Entry_UnknownSyscall,
    Entry_UserLevelFault,
    Entry_DebugFault,
    Entry_VMFault,
    Entry_Syscall,
    Entry_UnimplementedDevice,
    Entry_VCPUFault,
    Entry_Switch,
} entry_type_t;

char *entry_names[] = {
    "Entry_Interrupt",
    "Entry_UnknownSyscall",
    "Entry_UserLevelFault",
    "Entry_DebugFault",
    "Entry_VMFault",
    "Entry_Syscall",
    "Entry_UnimplementedDevice",
    "Entry_VCPUFault",
    "Entry_Switch",
};

#define SEL4_PACKED             __attribute__((packed))
typedef uint64_t seL4_Word;

/**
 * @brief Kernel entry logging
 *
 * Encapsulates useful info about the cause of the kernel entry
 */
typedef struct SEL4_PACKED kernel_entry {
    seL4_Word path: 4;
    union {
        struct {
            seL4_Word core: 3;
            seL4_Word word: 26;
        };
        /* Tracked kernel entry info filled from outside this file */
        struct {
            seL4_Word syscall_no: 4;
            seL4_Word cap_type: 5;
            seL4_Word is_fastpath: 1;
            seL4_Word invocation_tag: 19;
        };
        void *next;
    };
} kernel_entry_t;

typedef struct benchmark_syscall_log_entry {
    uint64_t  start_time;
    uint32_t  duration;
    kernel_entry_t entry;
} benchmark_track_kernel_entry_t;

struct Page_Header {
	uint64_t timestamp;
	union {
		uint64_t commit;
		uint8_t  overwrite;
	};
	char data[4080];
} __attribute__((packed));

#define TYPE_LEN_TIME_EXTEND (30)

struct Entry_Header {
        uint32_t type_len :5;
        uint32_t time_delta  :27;
        uint32_t array[];
};

struct Event_Function {
	uint16_t common_type;           // offset:0;  size:2; signed:0;
	uint8_t  common_flags;          // offset:2;  size:1; signed:0;
	uint8_t  common_preempt_count;  // offset:3;  size:1; signed:0;
	int32_t  common_pid;            // offset:4;  size:4; signed:1;
	uint64_t ip;                    // offset:8;  size:8; signed:0;
	uint64_t parent_ip;             // offset:16; size:8; signed:0;
} __attribute__((packed));

#define FLAGS_BIG_ENDIAN (1 << 0)
struct CTX {
	size_t   ptr;
	uint32_t flags;
	uint64_t curr_time;
	uint32_t filter_types;
	
	char   *data;
	size_t  size;
};

#define TS_SHIFT        (27)
#define TIME_DELTA_MASK (0xf8000000)
#define SEL4_PID	(9999)
#define SEL4_STR	"9999 seL4 kernel"
#define SEL4_STR_SIZE	(sizeof(SEL4_STR))

bool time_overflow(struct CTX *ctx, uint32_t time_delta) {
	return (time_delta & TIME_DELTA_MASK);
}

int write_data(struct CTX *ctx, struct Page_Header *page_header, void *data, size_t size) {
	pr_dbg("ptr: %ld size: %ld data remain: %ld", ctx->ptr, size,
			sizeof(page_header->data) - (ctx->ptr + size));

	if ((ctx->ptr + size) >= sizeof(page_header->data))
		return -1;

	memcpy(page_header->data + ctx->ptr, data, size);
	ctx->ptr += size;

	return 0;
}

int add_time_extend(struct CTX *ctx, struct Page_Header *page_header, const benchmark_track_kernel_entry_t *const entry) {
	int err = 0;
	uint32_t *time_ext;
	struct Entry_Header *entry_header;
	uint32_t time_delta = entry->start_time - ctx->curr_time;

	if (!time_overflow(ctx, time_delta))
		return err;
	pr_dbg("time_delta: 0x%08x masked: 0x%08x", time_delta, (time_delta & TIME_DELTA_MASK));

	entry_header = malloc(sizeof(struct Entry_Header) + sizeof(uint32_t));
	assert(entry_header);
	time_ext = (uint32_t *)entry_header->array;

	entry_header->type_len = TYPE_LEN_TIME_EXTEND;
	entry_header->time_delta = time_delta & ~TIME_DELTA_MASK;
	*time_ext = (time_delta & TIME_DELTA_MASK) >> TS_SHIFT;

	pr_dbg("%08x%08x", *(uint32_t *)entry_header, *time_ext);

	err = write_data(ctx, page_header, entry_header, sizeof(struct Entry_Header) + sizeof(uint32_t));

	if (!err)
		ctx->curr_time = entry->start_time;

	free(entry_header);

	return err;
}

int add_entry(struct CTX *ctx, struct Page_Header *page_header, const benchmark_track_kernel_entry_t *const entry) {
	int err = 0;
	struct Entry_Header *entry_header;
	struct Event_Function *event_function;
	uint32_t time_delta =  entry->start_time - ctx->curr_time;

	pr_dbg("%25s -- %12ld -- %12d -- 0x%10lx", 
		entry->entry.path < ARRAY_SIZE(entry_names) ? 
		entry_names[entry->entry.path] : "Wrong_Entry_Type",
		entry->start_time, entry->duration, (uintptr_t)entry->entry.next);

	pr_dbg("curr: %08lu time_delta: %08u masked: %08u", ctx->curr_time, time_delta, (time_delta & TIME_DELTA_MASK));

	entry_header = malloc(sizeof(struct Entry_Header) + sizeof(struct Event_Function));
	assert(entry_header);
	memset(entry_header, 0, sizeof(struct Entry_Header) + sizeof(struct Event_Function));
	event_function = (struct Event_Function *)entry_header->array;

	entry_header->type_len   = sizeof(struct Event_Function) / 4;
	entry_header->time_delta = time_delta & ~TIME_DELTA_MASK;

	event_function->common_pid	= SEL4_PID;
	event_function->common_type	= 1;
	event_function->common_preempt_count = 1;
	event_function->ip		= 0xfffffff0100c7910;
	event_function->parent_ip	= 0xfffffff0100c7900;

	err = write_data(ctx, page_header, entry_header, sizeof(struct Entry_Header) + sizeof(struct Event_Function));

	if (!err)
		ctx->curr_time = entry->start_time;

	free(entry_header);

	return err;
}

void page_header_init(struct CTX *ctx, struct Page_Header *page_header, 
		const benchmark_track_kernel_entry_t *const entry) {
	memset(page_header, 0, sizeof(struct Page_Header));

	ctx->ptr               = 0;
	ctx->curr_time         = entry->start_time;
	page_header->timestamp = entry->start_time;
	page_header->commit    = 0x02c8;

	pr_dbg("ptr: %04ld flags: 0x%04x time: %12ld", ctx->ptr, ctx->flags, ctx->curr_time);
}

bool filter_entry(struct CTX *ctx, const benchmark_track_kernel_entry_t *const entry) {
	return !(ctx->filter_types & BIT(entry->entry.path));
}

bool update_data(struct CTX *ctx, struct Page_Header *page_header) {
	pr_dbg("data size before: 0x%lx", ctx->size);
	ctx->data = realloc(ctx->data, ctx->size + sizeof(struct Page_Header));
	assert(ctx->data);

	memcpy(ctx->data + ctx->size, page_header, sizeof(struct Page_Header));
	ctx->size += sizeof(struct Page_Header);
	pr_dbg("data size after: 0x%lx", ctx->size);

	return true;
}

int main(int argc, char *argv[])
{
	int err;
	struct CTX ctx = {
		.ptr		= 0,
		.flags		= 0,
		.curr_time	= 0,
		.filter_types	= BIT(Entry_Switch),

		.data		= NULL,
		.size		= 0,
	};
	struct Page_Header page_header;
	benchmark_track_kernel_entry_t entry;

	if (argc < 3) {
		printf("%s [fname in] [fname out]\n", argv[0]);
		exit(0);
	}

	pr_dbg("%s: %s %s", argv[0], argv[1], argv[2]);

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		printf("cannot open file: %s", argv[1]);
		exit(0);
	}

	FILE *f_out = fopen(argv[2], "wb");
	if (!f_out) {
		printf("cannot open file: %s", argv[2]);
		exit(0);
	}


	for (;;) {
		err = fread(&entry, 1, sizeof(benchmark_track_kernel_entry_t), f);

		if (err != sizeof(benchmark_track_kernel_entry_t)) {
			pr_dbg("The end of file?\n");
			break;
		}

		if (filter_entry(&ctx, &entry))
			continue;

		page_header_init(&ctx, &page_header, &entry);
		err  = add_time_extend(&ctx, &page_header, &entry);
		err |= add_entry(&ctx, &page_header, &entry);

		if (err) {
			pr_dbg("Error cannot be here -- debug it!");
			exit(0);
		}

		while (true) {
			err = fread(&entry, 1, sizeof(benchmark_track_kernel_entry_t), f);

			if (!err) {
				pr_dbg("The end of file?");
				break;
			} else if (err != sizeof(benchmark_track_kernel_entry_t)) {
				pr_dbg("The file has been corrupted?");
				break;
			}

			if (filter_entry(&ctx, &entry))
				continue;

			err = add_time_extend(&ctx, &page_header, &entry);
			if (err)
				break;

			err = add_entry(&ctx, &page_header, &entry);
			if (err)
				break;
		}
		update_data(&ctx, &page_header);
	}

	printf("sizeof(struct Event_Function): %ld %ld \n", sizeof(struct Event_Function), sizeof(struct Event_Function) / 4);

	pr_dbg("Write file");
	err = fwrite(ctx.data, 1, ctx.size, f_out);
	assert(err == ctx.size);

	pr_dbg("Free data");
	free(ctx.data);

	fclose(f);
	fclose(f_out);

	return 0;
}
