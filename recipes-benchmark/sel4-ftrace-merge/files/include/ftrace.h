#ifndef FTRACE_H
#define FTRACE_H
#include <stdint.h>
#include <stddef.h>

#define read_sized_data(T, d, f) do			\
    {										\
        int err;							\
        d = malloc(sizeof(T));				\
        assert(d);							\
                                            \
        err = fread(d, 1, sizeof(T), f);	\
        assert(err == sizeof(T));			\
                                            \
        d = realloc(d, sizeof(T) + d->size);\
        assert(d);							\
                                            \
        err = fread(d->data, 1, d->size, f);\
        assert(err == d->size);				\
    } while(0)

typedef uint64_t u64;
typedef uint64_t local_t;

struct Ftrace_File {
    size_t size;
    struct Ftrace_Initial_wrap {
        size_t size;
        struct Ftrace_Initial *initial;
    } initial;
    struct Ftrace_Header_Page_Format_wrap {
        size_t size;
        struct Ftrace_Header_Page_Format *header_page_format;
    } header_page_format;
    struct Ftrace_Header_Event_Format_wrap {
        size_t size;
        struct Ftrace_Header_Event_Format *header_event_format;
    } header_event_format;
    struct Ftrace_Event_Formats_wrap {
        size_t size;
        struct Ftrace_Event_Formats *ftrace_event_formats;
    } ftrace_event_formats;
    struct Event_Formats_wrap {
        size_t size;
        struct Event_Formats *event_formats;
    } event_formats;
    struct KALLSYMS_Info_wrap {
        size_t size;
        struct KALLSYMS_Info *kallsyms_info;
    } kallsyms_info;
    struct PRINTK_Info_wrap {
        size_t size;
        struct PRINTK_Info *printk_info;
    } printk_info;
    struct PROCESS_Info_wrap {
        size_t size;
        struct PROCESS_Info *process_info;
    } process_info;
    struct Header_End_wrap {
        size_t size;
        struct Header_End *header_end;
    } header_end;
    struct Flyrecord_wrap {
        size_t size;
        struct Flyrecord *flyrecords;
    } flyrecords;
    struct Padding_wrap {
	size_t size;
	char *data;
    } padding;
    struct Rest_Data_wrap {
        size_t size;
        char *data;
    } rest_data;
};

struct Ftrace_Initial {
    uint8_t magic[3];
    union {
        struct {
            char tracing[7];
            char version[1];
            char nullt[1];
        } __attribute__((packed)) ver_dec;
        char ver_str[7 + 1 + 1];
    };
    uint8_t endianess;
    uint8_t bwidth;
    uint32_t pagesize;
}__attribute__((packed));

struct Ftrace_Header_Page_Format {
    char magic[12];
    uint64_t size;

    char data[];
}__attribute__((packed));

struct Ftrace_Header_Event_Format {
    char magic[13];
    uint64_t size;

    char data[];
}__attribute__((packed));

struct Ftrace_Event_Format {
        uint64_t size;

        char data[];
} __attribute__((packed));

struct Ftrace_Event_Formats {
    uint32_t count;

    char data[];
}__attribute__((packed));

struct Event_Formats {
    uint32_t count;

    char data[];
}__attribute__((packed));

struct Event_Format_Nxt{
        uint64_t size;
        char data[];
}__attribute__((packed)) header;

struct Event_Formats_Nxt {
    uint32_t count;

    char data[];
}__attribute__((packed));

struct KALLSYMS_Info {
    uint32_t size;

    char data[];
}__attribute__((packed));

struct PRINTK_Info {
    uint32_t size;

    char data[];
}__attribute__((packed));

struct PROCESS_Info {
    uint64_t size;

    char data[];
}__attribute__((packed));

struct CPU_Info {
    uint32_t size;

    struct {
        uint64_t offset;
        uint64_t size;
    } __attribute__((packed)) data[];
}__attribute__((packed));

struct Option_Data {
    uint32_t size;

    char data[];
}__attribute__((packed));

struct Option {
    uint16_t option;

    char data[];
};

struct Command {
    char name[10];
}__attribute__((packed));

struct Header_End {
    uint32_t n_cpu;

    char data[];
}__attribute__((packed));

struct Ftrace_Func_Field {
    unsigned short common_type;
    unsigned char common_flags;
    unsigned char common_preempt_count;
    int common_pid;

    unsigned long ip;
    unsigned long parent_ip;
} __attribute__((packed));

struct Page_Header {
        u64 timestamp;
    union {
            local_t commit;
            int overwrite;
    };
        char data[4080];
} __attribute__((packed));

struct Flyrecord {
    uint64_t offset;
    uint64_t size;
}__attribute__((packed));

#endif // FTRACE_H
