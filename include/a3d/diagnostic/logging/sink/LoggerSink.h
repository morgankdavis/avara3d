//
//  LoggerSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOGGERSINK_H
#define AVARA3D_LOGGERSINK_H


namespace a3d {

	class LoggerSink {

	public:

/*********************************************************************************************
	Public
 *********************************************************************************************/

		virtual ~LoggerSink() = 0;

		virtual void flush();
	};
}


#endif //AVARA3D_LOGGERSINK_H
