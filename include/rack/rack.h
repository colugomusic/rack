#pragma once

#define RACK_API_VERSION "0.0.1"

enum Rack_ParamFormatHint
{
    Rack_ParamFormatHint_Float = 0,
    Rack_ParamFormatHint_Hertz = 1,
    Rack_ParamFormatHint_Percentage = 2,
    Rack_ParamFormatHint_Toggle = 3,
    Rack_ParamFormatHint_Switch = 4,
};

#ifdef RACK_EXPORT_MODULE

#ifdef _WIN32
# define EXPORTED  __declspec( dllexport )
#else
# define EXPORTED
#endif

extern "C"
{
	// @return the rack API version
	EXPORTED const char* rack_get_api_version();

	/// @return the module name
	EXPORTED const char* rack_get_module_name();

	// @return the module version
	EXPORTED const char* rack_get_module_version();

	/// @return the module vendor name
	EXPORTED const char* rack_get_module_vendor();

	/// @return the number of units exposed by this module
	EXPORTED int rack_get_num_units();

	/// @param id the id of the unit to make. valid ids are zero to n-1 where n is the number
	/// returned from rack_get_num_units()
	/// @return the unit
	EXPORTED void* rack_unit_make(int id);

	/// @param handle the unit to free
	EXPORTED void rack_unit_free(void* handle);

	/// @param handle the unit
	/// @return the name of the unit
	EXPORTED const char* rack_unit_get_name(void* handle);

	/// @param handle the unit
	/// @param num_frames the number of frames to process
	EXPORTED void rack_unit_process(void* handle, int num_frames);

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
	/// @param id the id of the input channel to get. valid ids are zero to n-1 where n is
	/// the number returned from rack_unit_get_num_input_channels()
	/// @return the input channel
	EXPORTED void* rack_unit_get_input_channel(void* handle, int id);

	/// @param handle the unit
	/// @param id the id of the output channel to get. valid ids are zero to n-1 where n is
	/// the number returned from rack_unit_get_num_output_channels()
	/// @return the output channel
	EXPORTED void* rack_unit_get_output_channel(void* handle, int id);

	/// @param handle the unit
	/// @param id the id of the parameter to get. valid ids are zero to n-1 there n is the
	/// number returned from rack_unit_get_num_params
	/// @return the parameter
	EXPORTED void* rack_unit_get_param(void* handle, int id);

	/// @param handle the unit
	/// @param id the id of the trigger to get. valid ids are zero to n-1 there n is the
	/// number returned from rack_unit_get_num_triggers
	/// @return the trigger
	EXPORTED void* rack_unit_get_trigger(void* handle, int id);

	/// @param handle the parameter
	/// @return the name of the parameter
	EXPORTED const char* rack_param_get_name(void* handle);
	EXPORTED void rack_param_set_value(void* handle, float value);
	EXPORTED float rack_param_get_value(void* handle);
	EXPORTED Rack_ParamFormatHint rack_param_get_format_hint(void* handle);
	EXPORTED float rack_param_get_min(void* handle);
	EXPORTED float rack_param_get_max(void* handle);

	/// @param handle the channel
	/// @return the name of the channel
	EXPORTED const char* rack_channel_get_name(void* handle);
	EXPORTED void rack_channel_set_buffer_stride(void* handle, int stride);
	EXPORTED int rack_channel_get_buffer_stride(void* handle);
	EXPORTED char rack_channel_set_input_buffer(void* handle, const float* in);
	EXPORTED const float* rack_channel_get_input_buffer(void* handle);
	EXPORTED char rack_channel_set_output_buffer(void* handle, float* out);
	EXPORTED const float* rack_channel_get_output_buffer(void* handle);

	/// @param handle the trigger
	/// @return the name of the trigger
	EXPORTED const char* rack_trigger_get_name(void* handle);

	/// @param handle the trigger
	EXPORTED void rack_trigger_fire(void* handle);
}

#endif