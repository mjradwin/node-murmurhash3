/*!
 * node-murmurhash3
 * Copyright(c) 2013 Hideaki Ohno <hide.o.j55{at}gmail.com>
 * MIT Licensed
 */
#include <napi.h>
#include <sstream>
#include <iomanip>
#include "MurmurHash3.h"

/**
 * @brief Validate argument count and type, throwing on failure.
 * @param info CallbackInfo
 * @param index Argument index
 * @param typeName Name of expected type (for error message)
 * @param checker Lambda that returns true if argument is valid
 */
static void ValidateArg(const Napi::CallbackInfo& info, size_t index,
                         const char* typeName,
                         bool (*checker)(const Napi::Value&)) {
    Napi::Env env = info.Env();
    if (info.Length() < index + 1) {
        std::stringstream ss;
        ss << "A least " << index + 1 << " arguments are required";
        throw Napi::RangeError::New(env, ss.str());
    }
    if (!checker(info[index])) {
        std::stringstream ss;
        ss << "Argument " << index + 1 << " must be a " << typeName;
        throw Napi::TypeError::New(env, ss.str());
    }
}

static bool IsString(const Napi::Value& v) { return v.IsString(); }
static bool IsBoolean(const Napi::Value& v) { return v.IsBoolean(); }
static bool IsFunction(const Napi::Value& v) { return v.IsFunction(); }

/**
 * @brief Check if value is a valid Uint32 (number, non-negative, integer, <= UINT32_MAX)
 */
static bool IsUint32(const Napi::Value& v) {
    if (!v.IsNumber()) return false;
    double d = v.As<Napi::Number>().DoubleValue();
    return d >= 0 && d <= 4294967295.0 && d == (double)(uint32_t)d;
}

/**
 * @brief Make return value of murmur32
 */
static Napi::Value MakeReturnValue_murmur32(Napi::Env env, uint32_t hashValue, bool hexMode) {
    if (hexMode) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << hashValue;
        return Napi::String::New(env, ss.str());
    } else {
        return Napi::Number::New(env, hashValue);
    }
}

/**
 * @brief Make return value of murmur128
 */
static Napi::Value MakeReturnValue_murmur128(Napi::Env env, uint32_t* hashValue, bool hexMode) {
    if (hexMode) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 4; i++) {
            ss << std::setw(8) << hashValue[i];
        }
        return Napi::String::New(env, ss.str());
    } else {
        Napi::Array values = Napi::Array::New(env, 4);
        for (int i = 0; i < 4; i++) {
            values.Set((uint32_t)i, Napi::Number::New(env, hashValue[i]));
        }
        return values;
    }
}

/**
 * @brief Async worker for murmur32
 */
class Murmur32Worker : public Napi::AsyncWorker {
public:
    Murmur32Worker(Napi::Function& callback, std::string key, uint32_t seed, bool hexMode)
        : Napi::AsyncWorker(callback), key_(key), seed_(seed), hexMode_(hexMode), hashValue_(0) {
    }

    void Execute() override {
        MurmurHash3_x86_32(key_.c_str(), (int) key_.length(), seed_, &hashValue_);
    }

    void OnOK() override {
        Napi::Env env = Env();
        Callback().Call({env.Null(), MakeReturnValue_murmur32(env, hashValue_, hexMode_)});
    }

private:
    std::string key_;
    uint32_t seed_;
    bool hexMode_;
    uint32_t hashValue_;
};

/**
 * @brief Async worker for murmur128
 */
class Murmur128Worker : public Napi::AsyncWorker {
public:
    Murmur128Worker(Napi::Function& callback, std::string key, uint32_t seed, bool hexMode)
        : Napi::AsyncWorker(callback), key_(key), seed_(seed), hexMode_(hexMode) {
        memset(hashValue_, 0, sizeof(hashValue_));
    }

    void Execute() override {
        MurmurHash3_x86_128(key_.c_str(), (int) key_.length(), seed_, &hashValue_);
    }

    void OnOK() override {
        Napi::Env env = Env();
        Callback().Call({env.Null(), MakeReturnValue_murmur128(env, hashValue_, hexMode_)});
    }

private:
    std::string key_;
    uint32_t seed_;
    bool hexMode_;
    uint32_t hashValue_[4];
};

/**
 * @brief Calculate MurmurHash3(32bit) with async interface
 */
Napi::Value murmur32_async(const Napi::CallbackInfo& info) {
    ValidateArg(info, 0, "String", IsString);
    ValidateArg(info, 1, "Uint32", IsUint32);
    ValidateArg(info, 2, "Boolean", IsBoolean);
    ValidateArg(info, 3, "Function", IsFunction);

    std::string key = info[0].As<Napi::String>().Utf8Value();
    uint32_t seed = info[1].As<Napi::Number>().Uint32Value();
    bool hexMode = info[2].As<Napi::Boolean>().Value();
    Napi::Function cb = info[3].As<Napi::Function>();

    Murmur32Worker* worker = new Murmur32Worker(cb, key, seed, hexMode);
    worker->Queue();

    return info.Env().Undefined();
}

/**
 * @brief Calculate MurmurHash3(128bit) with async interface
 */
Napi::Value murmur128_async(const Napi::CallbackInfo& info) {
    ValidateArg(info, 0, "String", IsString);
    ValidateArg(info, 1, "Uint32", IsUint32);
    ValidateArg(info, 2, "Boolean", IsBoolean);
    ValidateArg(info, 3, "Function", IsFunction);

    std::string key = info[0].As<Napi::String>().Utf8Value();
    uint32_t seed = info[1].As<Napi::Number>().Uint32Value();
    bool hexMode = info[2].As<Napi::Boolean>().Value();
    Napi::Function cb = info[3].As<Napi::Function>();

    Murmur128Worker* worker = new Murmur128Worker(cb, key, seed, hexMode);
    worker->Queue();

    return info.Env().Undefined();
}

/**
 * @brief Calculate MurmurHash3(32bit) with sync interface
 */
Napi::Value murmur32_sync(const Napi::CallbackInfo& info) {
    ValidateArg(info, 0, "String", IsString);
    ValidateArg(info, 1, "Uint32", IsUint32);
    ValidateArg(info, 2, "Boolean", IsBoolean);

    std::string key = info[0].As<Napi::String>().Utf8Value();
    uint32_t seed = info[1].As<Napi::Number>().Uint32Value();
    bool hexMode = info[2].As<Napi::Boolean>().Value();

    uint32_t out;
    MurmurHash3_x86_32(key.c_str(), (int) key.length(), seed, &out);

    return MakeReturnValue_murmur32(info.Env(), out, hexMode);
}

/**
 * @brief Calculate MurmurHash3(128bit) with sync interface
 */
Napi::Value murmur128_sync(const Napi::CallbackInfo& info) {
    ValidateArg(info, 0, "String", IsString);
    ValidateArg(info, 1, "Uint32", IsUint32);
    ValidateArg(info, 2, "Boolean", IsBoolean);

    std::string key = info[0].As<Napi::String>().Utf8Value();
    uint32_t seed = info[1].As<Napi::Number>().Uint32Value();
    bool hexMode = info[2].As<Napi::Boolean>().Value();

    uint32_t out[4];
    MurmurHash3_x86_128(key.c_str(), (int) key.length(), seed, &out);

    return MakeReturnValue_murmur128(info.Env(), out, hexMode);
}

/**
 * @brief Initialize module
 */
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("murmur32", Napi::Function::New(env, murmur32_async));
    exports.Set("murmur128", Napi::Function::New(env, murmur128_async));
    exports.Set("murmur32Sync", Napi::Function::New(env, murmur32_sync));
    exports.Set("murmur128Sync", Napi::Function::New(env, murmur128_sync));
    return exports;
}

NODE_API_MODULE(murmurhash3, Init)
