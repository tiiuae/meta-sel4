#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "include/ftrace.h"

#define print_dbg(...) do { if (dbg_print) printf("[DBG]: " __VA_ARGS__); fflush(stdout); }while(0)
#define print_err(...) do {                printf("[ERR]: " __VA_ARGS__); fflush(stdout); }while(0)
#define print_inf(...)                	   printf("[INF]: " __VA_ARGS__)

static int dbg_print;
static char *pname;
static char *ftrace_file;
static char *sel4_file;
static char *sel4_tracebuffer_file;


void ftrace_merge_parse_args(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: %s [fname] [fname]\n", argv[0]);
        exit(0);
    }

    pname = argv[0];
    ftrace_file = argv[1];
    sel4_file = argv[2];
    sel4_tracebuffer_file = NULL;
    dbg_print = 1;
}

void *update_memory(void *mem, void *src, size_t *ptr, size_t size)
{
    char *_mem = mem;
    _mem = realloc(_mem, (*ptr) + size);
    assert(_mem);

    memcpy(_mem + (*ptr), src, size);
    *ptr += size;

    return _mem;
}

struct Ftrace_Initial_wrap read_ftrace_initial(FILE *f, size_t *const size)
{
    int err;
    struct Ftrace_Initial *init = malloc(sizeof(struct Ftrace_Initial));
    assert(init);

    print_dbg("\n%s: call\n", __func__);

    err = fread(init, 1, sizeof(struct Ftrace_Initial), f);
    assert(err == sizeof(struct Ftrace_Initial));

    print_dbg("------------- Initial Format -------------\n");

    print_dbg("Magic: 0x%x 0x%x 0x%x\n", init->magic[0], init->magic[1], init->magic[2]);
    assert(init->magic[0] == 0x17);
    assert(init->magic[1] == 0x08); // #TODO move to .h
    assert(init->magic[2] == 0x44);

    print_dbg("Trace: %s\n", init->ver_str);
    print_dbg("Version: %c\n", init->ver_dec.version[0]);
    assert(!memcmp(init->ver_dec.tracing, "tracing", sizeof(init->ver_dec.tracing)));
    assert(init->ver_dec.nullt[0] == '\0');

    print_dbg("Endianess: %s endian\n", init->endianess == 0 ? "little" : "big");
    print_dbg("sizeof(Word): %d\n", init->bwidth);
    print_dbg("sizeof(Page): %u\n", init->pagesize);

    print_dbg("------------- Initial Format -------------\n\n");

    *size += sizeof(struct Ftrace_Initial);
    assert(*size == ftell(f));

    struct Ftrace_Initial_wrap wrap =
    {
        .size = sizeof(struct Ftrace_Initial),
        .initial = init
    };
    return wrap;
}

struct Ftrace_Header_Page_Format_wrap
read_ftrace_header_page_format(FILE *f, size_t *size)
{
    struct Ftrace_Header_Page_Format *page_format;

    print_dbg("\n%s: call\n", __func__);

    read_sized_data(struct Ftrace_Header_Page_Format, page_format, f);

    print_dbg("------------- Header Page Format -------------\n");
    print_dbg("Header page format magic: %s\n", page_format->magic);
    assert(!memcmp(page_format->magic, "header_page",
                sizeof(page_format->magic)));

    print_dbg("Header page format size: %lu\n", page_format->size);
    print_dbg("Page Header Format:\n%.*s\n", (int)page_format->size, page_format->data);
    print_dbg("------------- Header Page Format -------------\n\n");

    *size += sizeof (struct Ftrace_Header_Page_Format) +
                page_format->size;
    assert(*size == ftell(f));

    struct Ftrace_Header_Page_Format_wrap wrap = {
        .size = sizeof (struct Ftrace_Header_Page_Format) +
                page_format->size,
        .header_page_format = page_format
    };
    return wrap;
}

struct Ftrace_Header_Event_Format_wrap
read_ftrace_header_event_format(FILE *f, size_t *size)
{
    struct Ftrace_Header_Event_Format *event_format;

    print_dbg("\n%s: call\n", __func__);

    read_sized_data(struct Ftrace_Header_Event_Format, event_format, f);

    print_dbg("------------- Header Event Format -------------\n");
    print_dbg("Header event magic: %s\n", event_format->magic);
    assert(!memcmp(event_format->magic, "header_event",
                sizeof(event_format->magic)));

    print_dbg("Header event size: %lu\n", event_format->size);
    print_dbg("Header Event Format:\n%.*s\n", (int)event_format->size, event_format->data);
    print_dbg("------------- Header Event Format -------------\n\n");

    *size += sizeof(struct Ftrace_Header_Event_Format) +
                event_format->size;
    assert(*size == ftell(f));

    struct Ftrace_Header_Event_Format_wrap wrap = {
        .size = sizeof(struct Ftrace_Header_Event_Format) +
                event_format->size,
        .header_event_format = event_format
    };
    return wrap;
}

struct Ftrace_Event_Formats_wrap read_ftrace_event_formats(FILE *f, size_t *size)
{
    int err;
    size_t ptr = 0;
    struct Ftrace_Event_Formats *event_formats;

    print_dbg("\n%s: call\n", __func__);

    event_formats = malloc(sizeof(struct Ftrace_Event_Formats));
    assert(event_formats);

    err = fread(event_formats, 1, sizeof(struct Ftrace_Event_Formats), f);
    assert(err == sizeof (struct Ftrace_Event_Formats));
    ptr += err;

    print_dbg("------------- Ftrace Event Formats -------------\n");
    print_dbg("Ftrace event formats count: %u\n", event_formats->count);


    for (int i = 0; i != event_formats->count; ++i) {
        size_t add_size;
        struct Ftrace_Event_Format *_event_format;

        read_sized_data(struct Ftrace_Event_Format, _event_format, f);

        print_dbg("Event Format #%d size: 0x%lx\n", i, _event_format->size);
        print_dbg("Event Format:\n%.*s\n", (int)_event_format->size, _event_format->data);

        add_size = sizeof(struct Ftrace_Event_Format) + _event_format->size;
        event_formats = update_memory(event_formats, _event_format,
                                      &ptr, add_size);

        free(_event_format);
    }

    print_dbg("------------- Ftrace Event Formats -------------\n\n");

    struct Ftrace_Event_Formats_wrap wrap = {
        .size = ptr,
        .ftrace_event_formats = event_formats
    };
    *size += wrap.size;
    assert(*size == ftell(f));
    return wrap;
}

size_t read_nullt_string(char **buf, FILE *f) {

    char c;
    int err;
    size_t size = 0;
    char *str = NULL;

    do {
        size++;

        str = realloc(str, size);
        assert(str);

        err = fread(&c, 1, 1, f);
        assert(err);
        str[size - 1] = c;
    } while(c && size < 30);

    *buf = str;
    return size;
}

struct Event_Formats_Nxt
*read_event_formats_nxt(FILE *f, size_t *count)
{
    int err;
    size_t ptr = 0;
    struct Event_Formats_Nxt *event_formats;

    event_formats = malloc(sizeof(struct Event_Formats_Nxt));
    assert(event_formats);

    err = fread(event_formats, 1, sizeof(struct Event_Formats_Nxt), f);
    assert(err == sizeof (struct Event_Formats_Nxt));
    ptr += err;

    print_dbg("------------- subEvent Formats -------------\n");
    print_dbg("subEvent formats count: %u\n", event_formats->count);


    for (int i = 0; i != event_formats->count; ++i) {
        size_t add_size;
        struct Ftrace_Event_Format *_event_format;

        read_sized_data(struct Ftrace_Event_Format, _event_format, f);

        print_dbg("subEvent Format #%d size: 0x%lx\n", i, _event_format->size);
        print_dbg("subEvent Format:\n%.*s\n", (int)_event_format->size, _event_format->data);

        add_size = sizeof(struct Ftrace_Event_Format) + _event_format->size;
        event_formats = update_memory(event_formats, _event_format, &ptr, add_size);

        free(_event_format);
    }

    print_dbg("------------- subEvent Formats -------------\n\n");

    *count = ptr;
    return event_formats;
}

struct Event_Formats_wrap read_event_formats(FILE *f, size_t *size)
{
    int err;
    size_t ptr = 0;
    struct Event_Formats *event_formats;

    print_dbg("\n%s: call\n", __func__);

    event_formats = malloc(sizeof(struct Event_Formats));
    assert(event_formats);

    err = fread(event_formats, 1, sizeof(struct Event_Formats), f);
    assert(err == sizeof (struct Event_Formats));
    ptr += err;

    print_dbg("------------- Event Formats -------------\n");
    print_dbg("Event formats count: %u\n", event_formats->count);

    for (int i = 0; i != event_formats->count; ++i) {
        char *str = NULL;
        size_t add_size;

        add_size = read_nullt_string(&str, f);

        print_dbg("Event format #%d system name: %s\n", i, str);
        event_formats = update_memory(event_formats, str, &ptr, add_size);

        add_size = 0;
        struct Event_Formats_Nxt *nxt =
                read_event_formats_nxt(f, &add_size);
        event_formats = update_memory(event_formats, nxt, &ptr, add_size);

        free(nxt);
        free(str);
    }

    print_dbg("------------- Event Formats -------------\n\n");

    struct Event_Formats_wrap wrap = {
        .size = ptr,
        .event_formats = event_formats
    };
    *size += wrap.size;
    assert(*size == ftell(f));
    return wrap;
}

struct KALLSYMS_Info_wrap read_kallsyms_info(FILE * f, size_t *size)
{
    struct KALLSYMS_Info *data;

    print_dbg("\n%s: call\n", __func__);

    read_sized_data(struct KALLSYMS_Info, data, f);

    printf("------------- KALLSYMS Info -------------\n");
    printf("KALLSYMS Info size: 0x%x\n", data->size);
    printf("KALLSYMS Info data:\n%.*s\n", data->size, data->data);
    printf("------------- KALLSYMS Info -------------\n\n");

    struct KALLSYMS_Info_wrap wrap = {
        .size = sizeof(struct KALLSYMS_Info) + data->size,
        .kallsyms_info = data
    };
    *size += wrap.size;
    assert(*size == ftell(f));
    return wrap;
}

struct PRINTK_Info_wrap read_printk_info(FILE * f, size_t *size)
{
    struct PRINTK_Info *data;

    print_dbg("\n%s: call\n", __func__);

    read_sized_data(struct PRINTK_Info, data, f);

    printf("------------- PRINTK Info -------------\n");
    printf("PRINTK Info size: 0x%x\n", data->size);
    printf("PRINTK Info data:\n%.*s\n", data->size, data->data);
    printf("------------- PRINTK Info -------------\n\n");

    struct PRINTK_Info_wrap wrap = {
        .size = sizeof(struct PRINTK_Info) + data->size,
        .printk_info = data
    };
    *size += wrap.size;
    assert(*size == ftell(f));
    return wrap;
}

struct PROCESS_Info_wrap read_process_info(FILE * f, size_t *size)
{
    struct PROCESS_Info *data;

    print_dbg("\n%s: call\n", __func__);

    read_sized_data(struct PROCESS_Info, data, f);

    printf("------------- PROCESS Info -------------\n");
    printf("PROCESS Info size: 0x%lx\n", data->size);
    printf("PROCESS Info data:\n%.*s\n", (int)data->size, data->data);
    printf("------------- PROCESS Info -------------\n\n");

    struct PROCESS_Info_wrap wrap = {
        .size = sizeof(struct PROCESS_Info) + data->size,
        .process_info = data
    };
    *size += wrap.size;
    assert(*size == ftell(f));
    return wrap;
}

struct Command *read_comand(FILE *f) {
    int err;
    struct Command *cmd;

    cmd = malloc(sizeof(struct Command));
    assert(cmd);

    err = fread(cmd, 1, sizeof(struct Command), f);
    assert(err == sizeof(struct Command));

    return cmd;
}

void read_latency_not_impl(FILE *f) {
    print_err("ASCII file format is not supported now!\n");

    exit(-1);
}

void *read_options(FILE *f, size_t *l)
{
    int err;
    void *ret = NULL;
    size_t ptr = 0;
    struct Option opt;
    struct Option_Data *data;

    while (1) {
        err = fread(&opt, 1, sizeof(struct Option), f);
        assert(err == sizeof(struct Option));


        print_dbg("Option: %02d ", opt.option);

        ret = update_memory(ret, &opt, &ptr, sizeof(struct Option));

        if (!opt.option) {
            print_dbg("last option\n");
            break;
        }

        read_sized_data(struct Option_Data, data, f);
        print_dbg("Option size: %d\n", data->size);
        ret = update_memory(ret, data, &ptr, sizeof(struct Option_Data) + data->size);

        free(data);
    }

    print_dbg("%s: exit\n", __func__);
    *l = ptr;
    return ret;
}

struct Header_End_wrap read_header_end(FILE *f, size_t *size)
{
    int err;
    size_t ptr = sizeof(struct Header_End);
    struct Header_End *header_end;

    print_dbg("\n%s: call\n", __func__);

    header_end = malloc(sizeof(struct Header_End));
    assert(header_end);

    err = fread(header_end, 1, sizeof(struct Header_End), f);
    assert(err == sizeof(struct Header_End));

    print_dbg("------------- Options -------------\n");
    print_dbg("CPU count: %u\n", header_end->n_cpu);

    while(1) {
        size_t add_size;
        struct Command *_cmd;

        _cmd = read_comand(f);
        header_end = update_memory(header_end, _cmd, &ptr,
                                   sizeof(struct Command));
        free(_cmd);

        const char *const name = (char *)header_end +
                (ptr - sizeof(struct Command));

        print_dbg("Next cmd: %s\n", name);

        if (strncmp(name, "options  ", 10) == 0) {
            print_dbg("read option: \n");
            void *_options = read_options(f, &add_size);

            header_end = update_memory(header_end, _options, &ptr, add_size);
            free(_options);
        } else if (strncmp(name, "latency  ", 10) == 0) {
            print_dbg("read latency: \n");
            read_latency_not_impl(f);
            break;
        } else if (strncmp(name, "flyrecord", 10) == 0) {
            print_dbg("read flyrecord: \n");
            break;
        } else {
            print_err("Wrong cmd found.. exit..\n");
            fseek(f, -10, SEEK_CUR);
            exit(-1);
        }
    }

    struct Header_End_wrap wrap = {
        .size = ptr,
        .header_end = header_end
    };
    *size += wrap.size;
    print_dbg("size: 0x%lx expected size: 0x%lx\n", *size, ftell(f));
    assert(*size == ftell(f));
    return wrap;
}

struct Flyrecord_wrap read_flyrecords(FILE *f, const uint32_t n_cpu, size_t *_size)
{
    int err = 0;
    const size_t size = sizeof(struct Flyrecord) * n_cpu;
    struct Flyrecord *flyrecords;

    print_dbg("\n%s: call\n", __func__);

    flyrecords = malloc(size);
    assert(flyrecords);

    err = fread(flyrecords, 1, size, f);
    assert(err == size);

    for (int i = 0; i != n_cpu; ++i)
        print_dbg("CPU %02d -- offset: 0x%lx size: 0x%lx\n",
                  i, flyrecords[i].offset, flyrecords[i].size);

    struct Flyrecord_wrap wrap = {
        .size = size,
        .flyrecords = flyrecords
    };
    *_size += wrap.size;
    assert(*_size == ftell(f));
    return wrap;
}

struct Padding_wrap read_padding(FILE *f, struct Ftrace_File *ftrace)
{
    int err;
    char *data;
    size_t size;
    const struct Flyrecord *flyrecords = ftrace->flyrecords.flyrecords;

    print_dbg("\n%s: call\n", __func__);

    /* #TODO: assumed that we have at least one CPU */
    size = flyrecords[0].offset - ftell(f);
    print_dbg("padding size: 0x%lx\n", size);

    data = malloc(size);
    assert(data);

    err = fread(data, 1, size, f);
    assert(err == size);

    struct Padding_wrap wrap = {
        .size = size,
        .data = data
    };
    ftrace->size += wrap.size;
    assert(ftrace->size == ftell(f));
    return wrap;
}

void check_data_integrity(FILE *f, const struct Ftrace_File *const ftrace)
{
    size_t size;
    const long int position = ftell(f);
    fseek(f, 0, SEEK_END);
    const long int end_position = ftell(f);
    fseek(f, position, SEEK_SET);

    print_dbg("\n%s: call\n", __func__);

    size = ftrace->event_formats.size +
            ftrace->flyrecords.size +
            ftrace->ftrace_event_formats.size +
            ftrace->header_end.size +
            ftrace->header_event_format.size +
            ftrace->header_page_format.size +
            ftrace->initial.size +
            ftrace->kallsyms_info.size +
            ftrace->printk_info.size +
            ftrace->process_info.size +
	    ftrace->padding.size +
            ftrace->rest_data.size;

    print_dbg("FTRACE size: %lu expected size: %lu\n", size, end_position);
    assert(end_position == size);
    print_dbg("FTRACE size: %lu expected size: %lu\n", size, ftrace->size);
    assert(size == ftrace->size);
}

struct Rest_Data_wrap read_rest_data(FILE *f, struct Ftrace_File *ftrace)
{
    int err;
    char *data;
    size_t size, expected_size;
    const uint32_t n_cpu = ftrace->header_end.header_end->n_cpu;
    const struct Flyrecord *flyrecords = ftrace->flyrecords.flyrecords;

    print_dbg("\n%s: call\n", __func__);

    const long int position = ftell(f);
    fseek(f, 0, SEEK_END);
    const long int end_position = ftell(f);
    fseek(f, position, SEEK_SET);

    size = end_position - position;
    expected_size = flyrecords[n_cpu - 1].offset +
            flyrecords[n_cpu - 1].size - flyrecords[0].offset;
    print_dbg("size of Rest: %lu expected size: %lu\n",
              size, expected_size);
    assert(size == expected_size);

    data = malloc(size);
    assert(data);

    err = fread(data, 1, size, f);
    assert(err == size);

    struct Rest_Data_wrap wrap = {
        .size = size,
        .data = data
    };
    ftrace->size += wrap.size;
    assert(ftrace->size == ftell(f));
    return wrap;
}

static void add_new_records(struct Ftrace_File *ftrace)
{
    int err;
    FILE *f = NULL;
    char *data = NULL;
    const uint32_t n_cpu = ftrace->header_end.header_end->n_cpu;
    struct Flyrecord *records = ftrace->flyrecords.flyrecords;
    struct Flyrecord new_record = {
	.offset = records[n_cpu -1].offset + records[n_cpu -1].size,
	.size   = 0,
    };

    print_dbg("\n%s: call\n", __func__);

    f = fopen(sel4_file, "rb");
    assert(f);

    fseek(f, 0, SEEK_END);
#if 1
    new_record.size = ftell(f);
#else
    new_record.size = records[n_cpu -1].size;
#endif
    fseek(f, 0, SEEK_SET);

    data = malloc(new_record.size);
    assert(data);

    err = fread(data, 1, new_record.size, f);
    assert(err == new_record.size);
    fclose(f);

    ftrace->flyrecords.flyrecords = realloc(ftrace->flyrecords.flyrecords, 
		    ftrace->flyrecords.size + sizeof(struct Flyrecord));
    assert(ftrace->flyrecords.flyrecords);

    memcpy(((char *)ftrace->flyrecords.flyrecords) + ftrace->flyrecords.size,
		    &new_record, sizeof(struct Flyrecord));
    ftrace->flyrecords.size += sizeof(struct Flyrecord);
    ftrace->header_end.header_end->n_cpu += 1;

    /* #TODO: btw, need to handle this one! */
    assert(ftrace->padding.size - sizeof(struct Flyrecord) > 0);
    ftrace->padding.size -= sizeof(struct Flyrecord);

    ftrace->rest_data.data = realloc(ftrace->rest_data.data,
		    ftrace->rest_data.size + new_record.size);
    assert(ftrace->rest_data.data);
#if 1
    memcpy(ftrace->rest_data.data + ftrace->rest_data.size, data, new_record.size);
#else
    memcpy(ftrace->rest_data.data + ftrace->rest_data.size, ftrace->rest_data.data, new_record.size);
#endif
    ftrace->rest_data.size += new_record.size;

    free(data);

    ftrace->size += new_record.size;

    print_dbg("n_cpu: %d\n", ftrace->header_end.header_end->n_cpu);
    print_dbg("new flyrecord: offset: 0x%lx size: 0x%lx\n",
		    new_record.offset, new_record.size);
}

// #TODO: here we need to parse for avalible PIDs, in future
#define SEL4_PID	(9999)
#define SEL4_STR	"9999 seL4_kernel\n"
#define SEL4_STR_SIZE	(sizeof(SEL4_STR) - 1)

void add_sel4_pid(struct Ftrace_File *ftrace)
{
    print_dbg("%s: call\n", __func__);

    ftrace->process_info.process_info = realloc(ftrace->process_info.process_info,
		    ftrace->process_info.size + SEL4_STR_SIZE);
    assert(ftrace->process_info.process_info);

#if 1
    memcpy(&ftrace->process_info.process_info->data[ftrace->process_info.process_info->size],
		    SEL4_STR, SEL4_STR_SIZE);

    ftrace->process_info.process_info->size += SEL4_STR_SIZE;
    ftrace->process_info.size += SEL4_STR_SIZE;
//    ftrace->size += SEL4_STR_SIZE;
#endif

    /* #TODO: btw, need to handle this one! */
    assert(ftrace->padding.size - SEL4_STR_SIZE > 0);
    ftrace->padding.size -= SEL4_STR_SIZE;

    printf("------------- PROCESS Info NEW-------------\n");
    printf("PROCESS Info full size: 0x%lx\n", ftrace->process_info.size);
    printf("PROCESS Info size: 0x%lx\n", ftrace->process_info.process_info->size);
    printf("PROCESS Info data:\n%.*s\n", (int)ftrace->process_info.process_info->size, 
		    ftrace->process_info.process_info->data);
    printf("------------- PROCESS Info NEW-------------\n\n");
}

#define SEL4_FUNC	"fffffff0100c7900 t seL4_kernel\nfffffff0100c7910 t Entry_Switch\n"
#define SEL4_FUNC_SIZE	(sizeof(SEL4_FUNC) - 1)

void add_sel4_func(struct Ftrace_File *ftrace)
{
    print_dbg("%s: call\n", __func__);

    ftrace->kallsyms_info.kallsyms_info = realloc(ftrace->kallsyms_info.kallsyms_info,
		    ftrace->kallsyms_info.size + SEL4_FUNC_SIZE);
    assert(ftrace->kallsyms_info.kallsyms_info);

#if 1
    memcpy(&ftrace->kallsyms_info.kallsyms_info->data[ftrace->kallsyms_info.kallsyms_info->size],
		    SEL4_FUNC, SEL4_FUNC_SIZE);

    ftrace->kallsyms_info.kallsyms_info->size += SEL4_FUNC_SIZE;
    ftrace->kallsyms_info.size += SEL4_FUNC_SIZE;
//    ftrace->size += SEL4_FUNC_SIZE;
#endif

    /* #TODO: btw, need to handle this one! */
    assert(ftrace->padding.size - SEL4_FUNC_SIZE > 0);
    ftrace->padding.size -= SEL4_FUNC_SIZE;

    printf("------------- KALLSYMS Info NEW-------------\n");
    printf("KALLSYMS Info full size: 0x%lx\n", ftrace->kallsyms_info.size);
    printf("KALLSYMS Info size: 0x%x\n", ftrace->kallsyms_info.kallsyms_info->size);
    printf("KALLSYMS Info data:\n%.*s\n", ftrace->kallsyms_info.kallsyms_info->size, 
		    ftrace->kallsyms_info.kallsyms_info->data);
    printf("------------- KALLSYMS Info NEW-------------\n\n");
}

void process_ftrace(struct Ftrace_File *ftrace)
{
    add_new_records(ftrace);
    add_sel4_pid(ftrace);
    add_sel4_func(ftrace);
}

void save_ftrace(struct Ftrace_File *ftrace)
{
    int err;
    FILE *f = fopen("./test_data.dat", "w");

    print_dbg("\n%s: call\n", __func__);

    print_dbg("Write: initial\n");
    err = fwrite(ftrace->initial.initial, 1, ftrace->initial.size, f);
    assert(err == ftrace->initial.size);

    print_dbg("Write: Header Page Format\n");
    err = fwrite(ftrace->header_page_format.header_page_format, 1,
                 ftrace->header_page_format.size, f);
    assert(err == ftrace->header_page_format.size);

    print_dbg("Write: Header Event Format\n");
    err = fwrite(ftrace->header_event_format.header_event_format, 1,
                 ftrace->header_event_format.size, f);
    assert(err == ftrace->header_event_format.size);

    print_dbg("Write: FTRACE event formats\n");
    err = fwrite(ftrace->ftrace_event_formats.ftrace_event_formats, 1,
                 ftrace->ftrace_event_formats.size, f);
    assert(err == ftrace->ftrace_event_formats.size);

    print_dbg("Write: events formats\n");
    err = fwrite(ftrace->event_formats.event_formats, 1,
                 ftrace->event_formats.size, f);
    assert(err == ftrace->event_formats.size);

    print_dbg("Write: KALLSYMS info\n");
    err = fwrite(ftrace->kallsyms_info.kallsyms_info, 1,
                 ftrace->kallsyms_info.size, f);
    assert(err == ftrace->kallsyms_info.size);

    print_dbg("Write: PRINTK info\n");
    err = fwrite(ftrace->printk_info.printk_info, 1,
                 ftrace->printk_info.size, f);
    assert(err == ftrace->printk_info.size);

    print_dbg("Write: Process info\n");
    err = fwrite(ftrace->process_info.process_info, 1,
                 ftrace->process_info.size, f);
    assert(err == ftrace->process_info.size);

    print_dbg("Write: Header end\n");
    err = fwrite(ftrace->header_end.header_end, 1,
                 ftrace->header_end.size, f);
    assert(err == ftrace->header_end.size);

    print_dbg("Write: Flyrecords\n");
    err = fwrite(ftrace->flyrecords.flyrecords, 1,
                 ftrace->flyrecords.size, f);
    assert(err == ftrace->flyrecords.size);

    print_dbg("Write: Padding\n");
    err = fwrite(ftrace->padding.data, 1,
                 ftrace->padding.size, f);
    assert(err == ftrace->padding.size);

    print_dbg("Write: Rest Data\n");
    err = fwrite(ftrace->rest_data.data, 1,
                 ftrace->rest_data.size, f);
    assert(err == ftrace->rest_data.size);

    check_data_integrity(f, ftrace);
    fclose(f);
}

void free_ftrace_data(struct Ftrace_File *ftrace)
{
    print_dbg("\n%s: call\n", __func__);

    free(ftrace->initial.initial);
    ftrace->initial.initial = NULL;

    free(ftrace->header_page_format.header_page_format);
    ftrace->header_page_format.header_page_format = NULL;

    free(ftrace->header_event_format.header_event_format);
    ftrace->header_event_format.header_event_format = NULL;

    free(ftrace->ftrace_event_formats.ftrace_event_formats);
    ftrace->ftrace_event_formats.ftrace_event_formats = NULL;

    free(ftrace->event_formats.event_formats);
    ftrace->event_formats.event_formats = NULL;

    free(ftrace->kallsyms_info.kallsyms_info);
    ftrace->kallsyms_info.kallsyms_info = NULL;

    free(ftrace->printk_info.printk_info);
    ftrace->printk_info.printk_info = NULL;

    free(ftrace->process_info.process_info);
    ftrace->process_info.process_info = NULL;

    free(ftrace->header_end.header_end);
    ftrace->header_end.header_end = NULL;

    free(ftrace->flyrecords.flyrecords);
    ftrace->flyrecords.flyrecords = NULL;

    free(ftrace->padding.data);
    ftrace->padding.data = NULL;

    free(ftrace->rest_data.data);
    ftrace->rest_data.data = NULL;

    memset(ftrace, 0, sizeof(struct Ftrace_File));
}

void read_ftrace_file(const char *fname) {
    struct Ftrace_File ftrace = {0};

    print_dbg("%s: try to open: %s\n\n", pname, ftrace_file);

    FILE *f = fopen(fname, "rb");
    assert(f);
    assert(ftrace.size == 0);

    ftrace.initial = read_ftrace_initial(f, &ftrace.size);
    ftrace.header_page_format = read_ftrace_header_page_format(f, &ftrace.size);
    ftrace.header_event_format = read_ftrace_header_event_format(f, &ftrace.size);
    ftrace.ftrace_event_formats = read_ftrace_event_formats(f, &ftrace.size);
    ftrace.event_formats = read_event_formats(f, &ftrace.size);
    ftrace.kallsyms_info = read_kallsyms_info(f, &ftrace.size);
    ftrace.printk_info = read_printk_info(f, &ftrace.size);
    ftrace.process_info = read_process_info(f, &ftrace.size);
    ftrace.header_end = read_header_end(f, &ftrace.size);
    ftrace.flyrecords = read_flyrecords(f, ftrace.header_end.header_end->n_cpu, &ftrace.size);
    ftrace.padding = read_padding(f, &ftrace);
    ftrace.rest_data = read_rest_data(f, &ftrace);

    check_data_integrity(f, &ftrace);

    process_ftrace(&ftrace);
    save_ftrace(&ftrace);

    free_ftrace_data(&ftrace);
}

int main(int argc, char *argv[])
{
    ftrace_merge_parse_args(argc, argv);

    read_ftrace_file(ftrace_file);

    return 0;
}
