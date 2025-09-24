/* We're going to use CTest since llparse-fixture has a hard time with \x00 in different frames */
#include "ctest.h"
#include "llh2.h"

/* Will use C++ for buffers since this spans can get a bit annoying... */
#include <string>
#include <stdint.h>

enum class FrameType {
    DATA = 0x00,
    HEADERS = 0x01,
    PRIORITY = 0x02,
    RST_STREAM = 0x03,
    SETTINGS = 0x04,
    PUSH_PROMISE = 0x05,
    PING = 0x06,
    GOAWAY = 0x07,
    WINDOW_UPDATE = 0x08,
    CONTINUATION = 0x09,
    ALTSVC = 0x0A

    
};

bool operator==(uint8_t t, FrameType ft){
    return (static_cast<FrameType>(t)) == ft; 
};

class DataFrame {
public:
    uint8_t flags;
    uint8_t pad_length;
    std::string body;
    uint32_t length;
    DataFrame(): flags(0) , pad_length(0), body(""), length(0){
    };
};

const char dataframe_payload[] = "\x00\x00\x08\x00\x01\x00\x00\x00\x01testdata";
const char dataframe_payload_with_padding[] = "\x00\x00\x13\x00\x09\x00\x00\x00\x01\x0Atestdata" \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

#define H2CALLBACK_TO_SPAN(PREFIX, NAME, CLASS) \
    int PREFIX##_on_##NAME##(llh2_t* parser, const char* data, size_t len){\
        ((reinterpret_cast<CLASS*>(parser->data))->##NAME##).append(data, len);\
        return 0; \
    }

#define H2CALLBACK(PREFIX, NAME, CLASS) \
    int PREFIX##_on_##NAME##_complete(llh2_t* parser){ \
        (reinterpret_cast<CLASS*>(parser->data))->##NAME = parser->##NAME; \
        return 0; \
    }

#define H2CALLBACK_SETTER(PREFIX, NAME, CLASS) \
    settings.on_##NAME##_complete = PREFIX##_on_##NAME##_complete;

#define H2CALLBACK_SPAN_SETTER(PREFIX, NAME, CLASS) \
    settings.on_##NAME = PREFIX##_on_##NAME##;


#define DATAFRAME_MAP(XX, SPAN) \
    XX(dataframe, flags, DataFrame) \
    XX(dataframe, pad_length, DataFrame) \
    XX(dataframe, length, DataFrame) \
    SPAN(dataframe, body, DataFrame) 

DATAFRAME_MAP(H2CALLBACK, H2CALLBACK_TO_SPAN)


CTEST_DATA(llh2_dataframe){
    llh2_t cparser; 
    llh2_settings_t settings;
    DataFrame df;
};

CTEST_SETUP(llh2_dataframe){
    llh2_t cparser; 
    llh2_settings_t settings;
    DataFrame df = DataFrame::DataFrame();
    cparser.data = reinterpret_cast<void*>(&df);

    llh2_settings_init(&settings);
    settings.on_flags_complete = dataframe_on_flags_complete; 
    settings.on_pad_length_complete = dataframe_on_pad_length_complete; 
    settings.on_length_complete = dataframe_on_length_complete; 
    settings.on_body = dataframe_on_body;
    llh2_init(&cparser, &settings);
};

CTEST_TEARDOWN(llh2_dataframe){};

CTEST2(llh2_dataframe, test1) 
{
    llh2_errno_t err = llh2_execute(&data->cparser, dataframe_payload, 17);
    /* Should've been paused at on_reset */
    ASSERT_EQUAL(err, H2PE_PAUSED);
    DataFrame* frame  = reinterpret_cast<DataFrame*>(data->cparser.data);
    ASSERT_TRUE(frame->flags & H2SF_END_STREAM);
    ASSERT_TRUE(frame->pad_length == 0);
    ASSERT_TRUE(frame->body == "testdata");
    ASSERT_TRUE(frame->body.length() == 8);
    ASSERT_TRUE(frame->length == 8);
    ASSERT_TRUE(data->cparser.type == FrameType::DATA);
    llh2_reset(&data->cparser);
};


CTEST2(llh2_dataframe, test2) 
{
    /* Chop it up into 2 different streams */
    llh2_errno_t err = llh2_execute(&data->cparser, "\x00\x00\x08\x00", 4);
    ASSERT_EQUAL(err, H2PE_OK);
    err = llh2_execute(&data->cparser, "\x01\x00\x00\x00\x01testdata", 13);

    /* Should've been paused at on_reset */
    ASSERT_EQUAL(err, H2PE_PAUSED);
    DataFrame* frame = reinterpret_cast<DataFrame*>(data->cparser.data);
    ASSERT_TRUE(frame->flags & H2SF_END_STREAM);
    ASSERT_TRUE(frame->pad_length == 0);
    ASSERT_TRUE(frame->body == "testdata");
    ASSERT_TRUE(frame->body.length() == 8);
    ASSERT_TRUE(frame->length == 8);
    ASSERT_TRUE(data->cparser.type == FrameType::DATA);
    llh2_reset(&data->cparser);
};


CTEST2(llh2_dataframe, test3) 
{
    llh2_errno_t err = llh2_execute(&data->cparser, dataframe_payload, 17);
    /* Should've been paused at on_reset */
    ASSERT_EQUAL(err, H2PE_PAUSED);
    DataFrame* frame = reinterpret_cast<DataFrame*>(data->cparser.data);
    ASSERT_TRUE(frame->flags & H2SF_END_STREAM);
    ASSERT_TRUE(frame->pad_length == 0);
    ASSERT_TRUE(frame->body == "testdata");
    ASSERT_TRUE(frame->body.length() == 8);
    ASSERT_TRUE(frame->length == 8);
    ASSERT_TRUE(data->cparser.type == FrameType::DATA);
    llh2_reset(&data->cparser);
};





