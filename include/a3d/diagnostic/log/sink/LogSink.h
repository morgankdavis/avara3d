//
//  LogSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOGSINK_H
#define AVARA3D_LOGSINK_H

namespace a3d {

	class LogSink {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		virtual ~LogSink() = 0;
		
/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		virtual void flush();
	};
}

#endif //AVARA3D_LOGSINK_H
