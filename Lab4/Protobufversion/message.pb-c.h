/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: message.proto */

#ifndef PROTOBUF_C_message_2eproto__INCLUDED
#define PROTOBUF_C_message_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1002001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Request Request;
typedef struct _Response Response;


/* --- enums --- */


/* --- messages --- */

struct _Request {
    ProtobufCMessage base;
    char *username;
    char *password;
    float value;
};
#define REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&request__descriptor) \
    , NULL, NULL, 0 }


struct _Response {
    ProtobufCMessage base;
    int32_t integer;
    int32_t decimal;
};
#define RESPONSE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&response__descriptor) \
    , 0, 0 }


/* Request methods */
void request__init
        (Request *message);

size_t request__get_packed_size
        (const Request *message);

size_t request__pack
        (const Request *message,
         uint8_t *out);

size_t request__pack_to_buffer
        (const Request *message,
         ProtobufCBuffer *buffer);

Request *
request__unpack
        (ProtobufCAllocator *allocator,
         size_t len,
         const uint8_t *data);

void request__free_unpacked
        (Request *message,
         ProtobufCAllocator *allocator);

/* Response methods */
void response__init
        (Response *message);

size_t response__get_packed_size
        (const Response *message);

size_t response__pack
        (const Response *message,
         uint8_t *out);

size_t response__pack_to_buffer
        (const Response *message,
         ProtobufCBuffer *buffer);

Response *
response__unpack
        (ProtobufCAllocator *allocator,
         size_t len,
         const uint8_t *data);

void response__free_unpacked
        (Response *message,
         ProtobufCAllocator *allocator);

/* --- per-message closures --- */

typedef void (*Request_Closure)
        (const Request *message,
         void *closure_data);

typedef void (*Response_Closure)
        (const Response *message,
         void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor request__descriptor;
extern const ProtobufCMessageDescriptor response__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_message_2eproto__INCLUDED */
