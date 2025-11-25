//
//  LoggerSink.cc
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//


#include "a3d/diagnostic/logging/sink/LoggerSink.h"


using namespace a3d;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

bool LoggerSink::classof(const LoggerSink*) {
	return true;
}

bool LoggerSink::classof(const LoggerSink& o) {
	return classof(&o);
}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

LoggerSink::~LoggerSink() { }

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

//void LoggerSink::flush() { }

LoggerSink::Kind LoggerSink::kind() const {
	return _kind;
}

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

LoggerSink::LoggerSink(LoggerSink::Kind k):
		_kind{k} {}
