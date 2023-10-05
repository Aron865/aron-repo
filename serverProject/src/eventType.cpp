#include "eventType.h"


static EErrorReason EER[] = {
	{EEC_SUCCESS,            "OK"},
	{EEC_INVALID_MSG,        "Invalid message"},
	{EEC_INVALID_DATA,       "Invalied data"},
	{EEC_METHOD_NOT_ALLOWED, "Method not allowed"},
	{EEC_PROCESS_FAILED,     "Process failed"},
	{EEC_BIKE_IS_TAKEN,       "Bike is taken"},
	{EEC_BIKE_IS_RUNNING,    "Bike is running"},
	{EEC_BIKE_IS_DAMAGED,    "Bike is damaged"},
    {EEC_NULL,               "Undefine"}
};
const char *getReasonByErrorCode(i32_t code)
{
    i32_t i = 0;
	for (i = 0; EER[i].code != EEC_NULL; i++) {
		if (EER[i].code == code) {
			return EER[i].reason;
		}
	}
	return EER[i].reason;
}