#include "pch.hpp"
#include "AudioSubmodule.hpp"
#include <AL/al.h>
#include <AL/alc.h>

using namespace Pit;

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)

static bool check_al_errors(const std::string& filename, const std::uint_fast32_t line) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
        switch (error) {
        case AL_INVALID_NAME:
            std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
            break;
        case AL_INVALID_ENUM:
            std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
            break;
        case AL_INVALID_VALUE:
            std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
            break;
        case AL_INVALID_OPERATION:
            std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
            break;
        case AL_OUT_OF_MEMORY:
            std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
            break;
        default:
            std::cerr << "UNKNOWN AL ERROR: " << error;
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}
template<typename alFunction, typename... Params>
static auto alCallImpl(const char* filename,
                const std::uint_fast32_t line,
                alFunction function,
                Params... params)
    ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool> {
    function(std::forward<Params>(params)...);
    return check_al_errors(filename, line);
}

template<typename alFunction, typename... Params>
static auto alCallImpl(const char* filename,
                const std::uint_fast32_t line,
                alFunction function,
                Params... params)
    ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))> {
    auto ret = function(std::forward<Params>(params)...);
    check_al_errors(filename, line);
    return ret;
}


#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

static bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device) {
    ALCenum error = alcGetError(device);
    if (error != ALC_NO_ERROR) {
        std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
        switch (error) {
        case ALC_INVALID_VALUE:
            std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
            break;
        case ALC_INVALID_DEVICE:
            std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
            break;
        case ALC_INVALID_CONTEXT:
            std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
            break;
        case ALC_INVALID_ENUM:
            std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
            break;
        case ALC_OUT_OF_MEMORY:
            std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
            break;
        default:
            std::cerr << "UNKNOWN ALC ERROR: " << error;
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

template<typename alcFunction, typename... Params>
static auto alcCallImpl(const char* filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ALCdevice* device,
                 Params... params)
    ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool> {
    function(std::forward<Params>(params)...);
    return check_alc_errors(filename, line, device);
}

template<typename alcFunction, typename ReturnType, typename... Params>
static auto alcCallImpl(const char* filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ReturnType& returnValue,
                 ALCdevice* device,
                 Params... params)
    ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool> {
    returnValue = function(std::forward<Params>(params)...);
    return check_alc_errors(filename, line, device);
}

static std::int32_t convert_to_int(char* buffer, std::size_t len) {
    std::int32_t a = 0;
    if (std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for (std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

static bool load_wav_file_header(std::ifstream& file,
                          std::uint8_t& channels,
                          std::int32_t& sampleRate,
                          std::uint8_t& bitsPerSample,
                          ALsizei& size) {
    char buffer[4];
    if (!file.is_open())
        return false;

    // the RIFF
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read RIFF" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "RIFF", 4) != 0) {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
        return false;
    }

    // the size of the file
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read size of file" << std::endl;
        return false;
    }

    // the WAVE
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read WAVE" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "WAVE", 4) != 0) {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
        return false;
    }

    // "fmt/0"
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read fmt/0" << std::endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read the 16" << std::endl;
        return false;
    }

    // PCM should be 1?
    if (!file.read(buffer, 2)) {
        std::cerr << "ERROR: could not read PCM" << std::endl;
        return false;
    }

    // the number of channels
    if (!file.read(buffer, 2)) {
        std::cerr << "ERROR: could not read number of channels" << std::endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read sample rate" << std::endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
        return false;
    }

    // ?? dafaq
    if (!file.read(buffer, 2)) {
        std::cerr << "ERROR: could not read dafaq" << std::endl;
        return false;
    }

    // bitsPerSample
    if (!file.read(buffer, 2)) {
        std::cerr << "ERROR: could not read bits per sample" << std::endl;
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);

    // data chunk header "data"
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read data chunk header" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "data", 4) != 0) {
        std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
        return false;
    }

    // size of data
    if (!file.read(buffer, 4)) {
        std::cerr << "ERROR: could not read data size" << std::endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if (file.eof()) {
        std::cerr << "ERROR: reached EOF on the file" << std::endl;
        return false;
    }
    if (file.fail()) {
        std::cerr << "ERROR: fail state set on the file" << std::endl;
        return false;
    }

    return true;
}

static char* load_wav(const std::string& filename,
               std::uint8_t& channels,
               std::int32_t& sampleRate,
               std::uint8_t& bitsPerSample,
               ALsizei& size) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
        return nullptr;
    }
    if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size)) {
        std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
        return nullptr;
    }

    char* data = new char[size];

    in.read(data, size);

    return data;
}

static bool get_available_devices(std::vector<std::string>& devicesVec, ALCdevice* device) {
    const ALCchar* devices;
    if (!alcCall(alcGetString, devices, device, nullptr, ALC_DEVICE_SPECIFIER))
        return false;

    const char* ptr = devices;

    devicesVec.clear();

    do {
        devicesVec.push_back(std::string(ptr));
        ptr += devicesVec.back().size() + 1;
    }
    while (*(ptr + 1) != '\0');

    return true;
}

void AudioSubmodule::Init() {
	PIT_PROFILE_FUNCTION();


	// "C:/Users/Peter/Downloads/iamtheprotectorofthissystem.wav"
    ALCdevice* openALDevice = alcOpenDevice(nullptr);
    if (!openALDevice)
        return;

    ALCcontext* openALContext;
    if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext) {
        std::cerr << "ERROR: Could not create audio context" << std::endl;
        return;
    }
    ALCboolean contextMadeCurrent = false;
    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
        || contextMadeCurrent != ALC_TRUE) {
        std::cerr << "ERROR: Could not make audio context current" << std::endl;
        return;
    }

    std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitsPerSample;
    char* soundData;
    ALsizei soundDataSize;
    soundData = load_wav("C:/Users/Peter/Downloads/iamtheprotectorofthissystem.wav", channels, sampleRate, bitsPerSample, soundDataSize);
    if (!soundData) {
        std::cerr << "ERROR: Could not load wav" << std::endl;
        return;
    }

    ALuint buffer;
    alCall(alGenBuffers, 1, &buffer);

    ALenum format;
    if (channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else {
        std::cerr
            << "ERROR: unrecognised wave format: "
            << channels << " channels, "
            << bitsPerSample << " bps" << std::endl;
        return;
    }

    alCall(alBufferData, buffer, format, soundData, soundDataSize, sampleRate);
    //soundData.clear(); // erase the sound in RAM
    delete soundData;

    ALuint source;
    alCall(alGenSources, 1, &source);
    alCall(alSourcef, source, AL_PITCH, 1);
    alCall(alSourcef, source, AL_GAIN, 1.0f);
    alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
    alCall(alSourcei, source, AL_LOOPING, AL_FALSE);
    alCall(alSourcei, source, AL_BUFFER, buffer);

    alCall(alSourcePlay, source);

    ALint state = AL_PLAYING;

    while (state == AL_PLAYING) {
        alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);
    }

    alCall(alDeleteSources, 1, &source);
    alCall(alDeleteBuffers, 1, &buffer);

    alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr);
    alcCall(alcDestroyContext, openALDevice, openALContext);

    ALCboolean closed;
    alcCall(alcCloseDevice, closed, openALDevice, openALDevice);
}

void AudioSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

}

void AudioSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

}