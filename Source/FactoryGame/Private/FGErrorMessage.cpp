// This file has been automatically generated by the Unreal Header Implementation tool

#include "FGErrorMessage.h"
#include "Internationalization/StringTableRegistry.h"

UFGErrorMessage::UFGErrorMessage() : Super() {
	this->mErrorMessage = INVTEXT("UNSET ERROR");
	this->mErrorResponse = EErrorResponse::ER_NonIntrusivePopup;
}
