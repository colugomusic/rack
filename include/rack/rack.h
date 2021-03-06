﻿#pragma once

/*
A RACK MODULE is a shared library which implements the C interface listed below.
A rack module consists of zero or more units.

A UNIT is an audio processor which consists of:
 - zero or more input channels
 - zero or more output channels
 - zero or more parameters
 - zero or more triggers

┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ rack module                                                                 ┃
┃ ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓ ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓     ┃
┃ ┃ unit                            ┃ ┃ unit                            ┃     ┃
┃ ┃ ┏━━━━━━━━━┓ ┏━━━━━━━━━┓         ┃ ┃ ┏━━━━━━━━━┓ ┏━━━━━━━━━┓         ┃     ┃
┃ ┃ ┃ channel ┃ ┃ channel ┃ ...     ┃ ┃ ┃ channel ┃ ┃ channel ┃ ...     ┃     ┃
┃ ┃ ┗━━━━━━━━━┛ ┗━━━━━━━━━┛         ┃ ┃ ┗━━━━━━━━━┛ ┗━━━━━━━━━┛         ┃     ┃
┃ ┃ ┏━━━━━━━━━━━┓ ┏━━━━━━━━━━━┓     ┃ ┃ ┏━━━━━━━━━━━┓ ┏━━━━━━━━━━━┓     ┃     ┃
┃ ┃ ┃ parameter ┃ ┃ parameter ┃ ... ┃ ┃ ┃ parameter ┃ ┃ parameter ┃ ... ┃     ┃
┃ ┃ ┗━━━━━━━━━━━┛ ┗━━━━━━━━━━━┛     ┃ ┃ ┗━━━━━━━━━━━┛ ┗━━━━━━━━━━━┛     ┃     ┃
┃ ┃ ┏━━━━━━━━━┓ ┏━━━━━━━━━┓         ┃ ┃ ┏━━━━━━━━━┓ ┏━━━━━━━━━┓         ┃     ┃
┃ ┃ ┃ trigger ┃ ┃ trigger ┃ ...     ┃ ┃ ┃ trigger ┃ ┃ trigger ┃ ...     ┃     ┃
┃ ┃ ┗━━━━━━━━━┛ ┗━━━━━━━━━┛         ┃ ┃ ┗━━━━━━━━━┛ ┗━━━━━━━━━┛         ┃ ... ┃
┃ ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛ ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛     ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

Functions marked with [no-audio] should be considered by hosts to be unsafe to call
from an audio processing thread.

Any function not marked with [no-audio] should be considered to be "audio-safe".

it is the module implementer's responsibility to ensure that audio-safe functions
do not perform any operations which may take an unbounded amount of time to execute.

Examples of such operations include:
 - Memory allocation / deallocation
 - I/O (e.g. printf())
 - Context switching (e.g. exec(), yield())
 - Mutex operations

*/

// modules should return this from rack_get_api_version()
#define RACK_API_VERSION "0.0.1"

// modules should process audio data in buffers of this size
#define RACK_VECTOR_SIZE 64

// returned from rack_param_get_format_hint(), indicating the type of a parameter.
// hosts are free to ignore this hint
enum Rack_ParamFormatHint
{
	Rack_ParamFormatHint_Float = 0,
	Rack_ParamFormatHint_Int = 1,
	Rack_ParamFormatHint_Hertz = 2,
	Rack_ParamFormatHint_Percentage = 3,
	Rack_ParamFormatHint_Milliseconds = 4,
	Rack_ParamFormatHint_Decibels = 5,
	Rack_ParamFormatHint_Toggle = 6,
	Rack_ParamFormatHint_Switch = 7,
};

#ifdef RACK_EXPORT_MODULE

#ifdef _WIN32
# define EXPORTED  __declspec( dllexport )
#else
# define EXPORTED
#endif

extern "C"
{
	// get the rack API version that this module implements. module implementers should
	// return RACK_API_VERSION
	// @return the rack API version
	EXPORTED const char* rack_get_api_version();

	// get the module name. the combination of module name and module version should be
	// unique to the vendor (as specified by rack_get_module_vendor())
	/// @return the module name
	EXPORTED const char* rack_get_module_name();

	// get the module version. the combination of module name and module version should be
	// unique to the vendor (as specified by rack_get_module_vendor())
	// @return the module version
	EXPORTED const char* rack_get_module_version();

	/// get the module vendor (developer) name
	/// @return the module vendor name
	EXPORTED const char* rack_get_module_vendor();

	// get the number of units exposed by this module. this number specifies the range of
	// valid unit ids for rack_unit_make()
	/// @return the number of units exposed by this module
	EXPORTED int rack_get_num_units();

	/// [no-audio]
	/// @param id the id of the unit to create. valid ids are 0..(n-1) where n is the number
	/// returned from rack_get_num_units()
	/// @return a handle to the new unit instance
	EXPORTED void* rack_unit_make(int id);

	/// [no-audio]
	/// free the specified unit. 
	/// it is the host's responsibility to ensure that the unit is not currently being 
	/// processed by any thread
	/// @param handle the unit to free
	EXPORTED void rack_unit_free(void* handle);

	/// @param handle the unit
	/// @return the name of the unit
	EXPORTED const char* rack_unit_get_name(void* handle);

	/// clear the unit.
	/// will most likely be called from an audio thread. module implementers should not  
	/// perform unbounded operations in this function.
	/// @param handle the unit
	EXPORTED void rack_unit_clear(void* handle);

	/// process the unit. 
	/// will most likely be called from an audio thread. module implementers should not  
	/// perform unbounded operations in this function.
	/// @param handle the unit
	EXPORTED void rack_unit_process(void* handle);

	/// @param handle the unit
	/// @return the current sample rate
	EXPORTED int rack_unit_get_sample_rate(void* handle);

	/// @param handle the unit
	/// @param sample_rate the new sample rate
	EXPORTED void rack_unit_set_sample_rate(void* handle, int sample_rate);

	/// @param handle the unit
	/// @return the number of parameters exposed by this unit
	EXPORTED int rack_unit_get_num_params(void* handle);

	/// @param handle the unit
	/// @return the number of input channels to this unit
	EXPORTED int rack_unit_get_num_input_channels(void* handle);

	/// @param handle the unit
	/// @return the number of output channels from this unit
	EXPORTED int rack_unit_get_num_output_channels(void* handle);

	/// @param handle the unit
	/// @return the number of triggers exposed by this unit
	EXPORTED int rack_unit_get_num_triggers(void* handle);

	/// @param handle the unit
	/// @param id the id of the input channel to get. valid ids are 0..(n-1) where n is
	/// the number returned from rack_unit_get_num_input_channels()
	/// @return the input channel
	EXPORTED void* rack_unit_get_input_channel(void* handle, int id);

	/// @param handle the unit
	/// @param id the id of the output channel to get. valid ids are 0..(n-1) where n is
	/// the number returned from rack_unit_get_num_output_channels()
	/// @return the output channel
	EXPORTED void* rack_unit_get_output_channel(void* handle, int id);

	/// @param handle the unit
	/// @param id the id of the parameter to get. valid ids are 0..(n-1) where n is the
	/// number returned from rack_unit_get_num_params
	/// @return the parameter
	EXPORTED void* rack_unit_get_param(void* handle, int id);

	/// @param handle the unit
	/// @param id the id of the trigger to get. valid ids are 0..(n-1) where n is the
	/// number returned from rack_unit_get_num_triggers
	/// @return the trigger
	EXPORTED void* rack_unit_get_trigger(void* handle, int id);

	/// @param handle the parameter
	/// @return the name of the parameter
	EXPORTED const char* rack_param_get_name(void* handle);

	/// set a static value for the parameter
	/// ignored if a value buffer is set with rack_param_set_value_buffer()
	/// @param handle the parameter
	/// @param value the new value for the parameter
	EXPORTED void rack_param_set_value(void* handle, float value);

	/// set the buffer from which value data will be read for this parmater in
	/// rack_unit_process().
	/// it is the host's responsibility to ensure that the buffer contains at least
	/// RACK_VECTOR_SIZE values and to ensure that the buffer stays alive for the
	/// duration of rack_unit_process(). 
	/// @param handle the parameter
	/// @param buffer the buffer
	EXPORTED void rack_param_set_value_buffer(void* handle, const float* buffer);

	/// @param handle the parameter
	/// @return the current value of the parameter
	EXPORTED float rack_param_get_value(void* handle);

	/// @param handle the parameter
	/// @return the default value for the parameter
	EXPORTED float rack_param_get_default_value(void* handle);

	/// @param handle the parameter
	/// @return a Rack_ParamFormatHint enum value indicating the type of the parameter. 
	/// hosts are free to ignore this hint
	EXPORTED Rack_ParamFormatHint rack_param_get_format_hint(void* handle);

	/// @param handle the parameter
	/// @return a value indicating the size of the parameter.
	/// 	1.0 = full size 
	/// 	0.5 = half size 
	/// 	2.0 = double size 
	/// hosts are free to ignore this hint
	EXPORTED float rack_param_get_size_hint(void* handle);

	/// @param handle the parameter
	/// @return the minimum value for the parameter. hosts must not exceed this limit.
	EXPORTED float rack_param_get_min(void* handle);

	/// @param handle the parameter
	/// @return the maximum value for the parameter. hosts must not exceed this limit.
	EXPORTED float rack_param_get_max(void* handle);

	/// get the name of a switch option when rack_param_get_format_hint() returns
	/// Rack_ParamFormatHint_Switch. valid ids are 0..n where n is the value returned
	/// from rack_param_get_max()
	/// @param handle the parameter
	/// @param id the id of the switch option
	/// @return the name of the switch option
	EXPORTED const char* rack_param_get_switch_option_name(void* handle, int id);

	/// @param handle the channel
	/// @return the name of the channel
	EXPORTED const char* rack_channel_get_name(void* handle);

	/// if the specified channel is an input channel, set the buffer from which data will
	/// be read in rack_unit_process(). 
	/// it is the host's responsibility to ensure that the buffer contains at least
	/// RACK_VECTOR_SIZE values and to ensure that the buffer stays alive for the duration
	/// of rack_unit_process(). 
	/// an input channel can be disabled by setting the input buffer to null. modules
	/// must support any configuration of enabled/disabled channels
	/// @param handle the channel
	/// @param in the buffer
	/// @return 0 if the specified channel is not an input channel. otherwise 1.
	EXPORTED char rack_channel_set_input_buffer(void* handle, const float* in);

	/// if the specified channel is an output channel, set the buffer to which data will
	/// be written in rack_unit_process(). 
	/// it is the host's responsibility to ensure that the buffer is large enough to
	/// hold at least RACK_VECTOR_SIZE values and to ensure that the buffer stays alive
	/// for the duration of rack_unit_process() 
	/// an output channel can be disabled by setting the output buffer to null. modules
	/// must support any configuration of enabled/disabled channels
	/// @param handle the channel
	/// @return 0 if the specified channel is not an output channel. otherwise 1.
	EXPORTED char rack_channel_set_output_buffer(void* handle, float* out);

	/// @param handle the channel
	/// @return the current input buffer for this channel
	EXPORTED const float* rack_channel_get_input_buffer(void* handle);

	/// @param handle the channel
	/// @return the current output buffer for this channel
	EXPORTED float* rack_channel_get_output_buffer(void* handle);

	/// @param handle the trigger
	/// @return the name of the trigger
	EXPORTED const char* rack_trigger_get_name(void* handle);

	/// @param handle the trigger
	EXPORTED void rack_trigger_fire(void* handle);
}

#endif