#include "sound.h"

#include "utility/static_defs.h"
#include "utility/unused.h"

#include <cassert>

#pragma warning(push, 0)
#include <dsound.h>
#pragma warning(pop)

/* ************** *
 * INTERNAL FUNCS *
 * ************** */
namespace blastbeat
{
	namespace sound
	{
        typedef LPDIRECTSOUNDBUFFER soundcard_handle;

        internal soundcard_handle
        get_soundcard_handle(LPDIRECTSOUND direct_sound, WAVEFORMATEX* sound_format_ptr);

		enum class sound_error { create_sound_buffer };

		internal void
		print_error(sound_error error_type, HRESULT error_val);
	}
}


global bool sound_system_initialized = false;

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pc_guid_device, LPDIRECTSOUND* ppds, LPUNKNOWN p_unk_outer)
typedef DIRECT_SOUND_CREATE(direct_sound_create_delegate);

void
blastbeat::sound::init(HWND window, uint32_t buffer_size, uint32_t samples_per_sec)
{
    // load the library
    HMODULE dsound_lib = LoadLibraryA("dsound.dll");
    assert(dsound_lib);

    // load the create routine
    #pragma warning(push)
    #pragma warning(disable : 4191)
    auto direct_sound_create =
        (direct_sound_create_delegate*) GetProcAddress(dsound_lib, "DirectSoundCreate");
    #pragma warning(pop)
    assert(direct_sound_create);
        
    // Get a direct sound obj
    LPDIRECTSOUND direct_sound;
    bool ds_create_success = SUCCEEDED(direct_sound_create(0, &direct_sound, 0));
    assert(ds_create_success);

    bool ds_coop_lvl_success = SUCCEEDED(direct_sound->SetCooperativeLevel(window, DSSCL_PRIORITY));
    assert(ds_coop_lvl_success);

    WAVEFORMATEX sound_format = {};
    sound_format.wFormatTag = WAVE_FORMAT_PCM; 
    sound_format.nChannels = 2;
    sound_format.nSamplesPerSec = samples_per_sec;
    sound_format.wBitsPerSample = 16;
    sound_format.nBlockAlign = (WORD) (sound_format.nChannels * sound_format.wBitsPerSample / 8);
    sound_format.nAvgBytesPerSec = sound_format.nBlockAlign * sound_format.nSamplesPerSec;
    sound_format.cbSize = 0;

    // Get a handle to the soundcard (primary buffer)
    get_soundcard_handle(direct_sound, &sound_format);


    // Get a sound buffer (secondary buffer)
    DSBUFFERDESC sound_buffer_description = {};
    sound_buffer_description.dwSize = sizeof(sound_buffer_description);
    sound_buffer_description.dwBufferBytes = buffer_size;
    sound_buffer_description.lpwfxFormat = &sound_format;

    LPDIRECTSOUNDBUFFER sound_buffer;
    bool ds_create_sound_buffer =
        SUCCEEDED(direct_sound->CreateSoundBuffer(&sound_buffer_description, &sound_buffer, 0));
    assert(ds_create_sound_buffer);

    // TODO (scott): Start playing it
}

internal blastbeat::sound::soundcard_handle
blastbeat::sound::get_soundcard_handle(LPDIRECTSOUND direct_sound, WAVEFORMATEX* sound_format_ptr)
{
    DSBUFFERDESC buf_description = {};
    buf_description.dwSize = sizeof(DSBUFFERDESC);
    buf_description.dwFlags = DSBCAPS_PRIMARYBUFFER;
    soundcard_handle sc_handle;
    bool get_soundcard_handle =
        SUCCEEDED(direct_sound->CreateSoundBuffer(&buf_description, &sc_handle, 0));
    assert(get_soundcard_handle);

    bool ds_primary_format = SUCCEEDED(sc_handle->SetFormat(sound_format_ptr));
    assert(ds_primary_format);

    return sc_handle;
}

internal void
blastbeat::sound::print_error(blastbeat::sound::sound_error error_type, HRESULT error_val)
{
    OutputDebugStringA("SOUND::PRINT_ERROR - ");
    switch (error_type)
    {
        case sound_error::create_sound_buffer:
            switch (error_val)
            {
				case DS_OK:
					OutputDebugStringA("DS_OK - "
						"The method succeeded.");
				break;
				case DS_NO_VIRTUALIZATION:
					OutputDebugStringA("DS_NO_VIRTUALIZATION - "
						"The buffer was created, but another 3D algorithm was substituted.");
				break;
				case DSERR_ACCESSDENIED:
					OutputDebugStringA("DSERR_ACCESSDENIED - "
						"The request failed because access was denied.");
				break;
				case DSERR_ALLOCATED:
					OutputDebugStringA("DSERR_ALLOCATED - "
						"The request failed because resources, such as a priority level, were already in use by another caller.");
				break;
				case DSERR_ALREADYINITIALIZED:
					OutputDebugStringA("DSERR_ALREADYINITIALIZED - "
						"The object is already initialized.");
				break;
				case DSERR_BADFORMAT:
					OutputDebugStringA("DSERR_BADFORMAT - "
						"The specified wave format is not supported.");
				break;
				case DSERR_BADSENDBUFFERGUID:
					OutputDebugStringA("DSERR_BADSENDBUFFERGUID - "
						"The GUID specified in an audiopath file does not match a valid mix-in buffer.");
				break;
				case DSERR_BUFFERLOST:
					OutputDebugStringA("DSERR_BUFFERLOST - "
						"The buffer memory has been lost and must be restored.");
				break;
				case DSERR_BUFFERTOOSMALL:
					OutputDebugStringA("DSERR_BUFFERTOOSMALL - "
						"The buffer size is not great enough to enable effects processing.");
				break;
				case DSERR_CONTROLUNAVAIL:
					OutputDebugStringA("DSERR_CONTROLUNAVAIL - "
						"The buffer control (volume, pan, and so on) requested by the caller is not available. Controls must be specified when the buffer is created, using the dwFlags member of DSBUFFERDESC.");
				break;
				case DSERR_DS8_REQUIRED:
					OutputDebugStringA("DSERR_DS8_REQUIRED - "
						"A DirectSound object of class CLSID_DirectSound8 or later is required for the requested functionality. For more information, see IDirectSound8 Interface.");
				break;
				case DSERR_FXUNAVAILABLE:
					OutputDebugStringA("DSERR_FXUNAVAILABLE - "
						"The effects requested could not be found on the system, or they are in the wrong order or in the wrong location; for example, an effect expected in hardware was found in software.");
				break;
				case DSERR_GENERIC:
					OutputDebugStringA("DSERR_GENERIC - "
						"An undetermined error occurred inside the DirectSound subsystem.");
				break;
				case DSERR_INVALIDCALL:
					OutputDebugStringA("DSERR_INVALIDCALL - "
						"This function is not valid for the current state of this object.");
				break;
				case DSERR_INVALIDPARAM:
					OutputDebugStringA("DSERR_INVALIDPARAM - "
						"An invalid parameter was passed to the returning function.");
				break;
				case DSERR_NOAGGREGATION:
					OutputDebugStringA("DSERR_NOAGGREGATION - "
						"The object does not support aggregation.");
				break;
				case DSERR_NODRIVER:
					OutputDebugStringA("DSERR_NODRIVER - "
						"No sound driver is available for use, or the given GUID is not a valid DirectSound device ID.");
				break;
				case DSERR_NOINTERFACE:
					OutputDebugStringA("DSERR_NOINTERFACE - "
						"The requested COM interface is not available.");
				break;
				case DSERR_OBJECTNOTFOUND:
					OutputDebugStringA("DSERR_OBJECTNOTFOUND - "
						"The requested object was not found.");
				break;
				case DSERR_OTHERAPPHASPRIO:
					OutputDebugStringA("DSERR_OTHERAPPHASPRIO - "
						"Another application has a higher priority level, preventing this call from succeeding.");
				break;
				case DSERR_OUTOFMEMORY:
					OutputDebugStringA("DSERR_OUTOFMEMORY - "
						"The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.");
				break;
				case DSERR_PRIOLEVELNEEDED:
					OutputDebugStringA("DSERR_PRIOLEVELNEEDED - "
						"A cooperative level of DSSCL_PRIORITY or higher is required.");
				break;
				case DSERR_SENDLOOP:
					OutputDebugStringA("DSERR_SENDLOOP - "
						"A circular loop of send effects was detected.");
				break;
				case DSERR_UNINITIALIZED:
					OutputDebugStringA("DSERR_UNINITIALIZED - "
						"The IDirectSound8::Initialize method has not been called or has not been called successfully before other methods were called.");
				break;
				case DSERR_UNSUPPORTED:
					OutputDebugStringA("DSERR_UNSUPPORTED - "
						"The function called is not supported at this time.");
				break;
				default:
					OutputDebugStringA("HRESULT ERROR CODE NOT FOUND");
				break;
            }
        break;

        default:
            OutputDebugStringA("Unrecognized error_type\n");
    }
}
