//
// Created by mkd on 11/5/23.
//

#include "ConvexDecomposer.h"

#include "GeometryElement.h"


using namespace ae;
using namespace std;
using namespace VHACD;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

ConvexDecomposer::ConvexDecomposer(vector<shared_ptr<GeometryElement>>&	elements,
								   Options& options,
								   bool runAsync) {

}


/*********************************************************************************************
	Public
 *********************************************************************************************/

void ConvexDecomposer::run() {

}

bool ConvexDecomposer::isRunning() const {

}

bool ConvexDecomposer::isAsync() const {

}

std::vector<std::shared_ptr<GeometryElement>>& ConvexDecomposer::sourceElements() const {

}

std::vector<std::shared_ptr<GeometryElement>>& ConvexDecomposer::decomposedElements() const {

}


ConvexDecomposer::ProgressFunction ConvexDecomposer::progressCallback() const {

}

void ConvexDecomposer::progressCallback(ProgressFunction function) {

}


ConvexDecomposer::FinishedFunction ConvexDecomposer::finishedCallback() const {

}

void ConvexDecomposer::finishedCallback(FinishedFunction function) {

}


ConvexDecomposer::CanceledFunction ConvexDecomposer::canceledCallback() const {

}

void ConvexDecomposer::canceledCallback(CanceledFunction function) {

}

/*********************************************************************************************
	IVHACD::IUserCallback
 *********************************************************************************************/

void ConvexDecomposer::Update(const double overallProgress,
			const double stageProgress,
			const char* const stage,
			const char* operation) {

}

void ConvexDecomposer::NotifyVHACDComplete() {

}

/*********************************************************************************************
	IVHACD::IUserLogger
 *********************************************************************************************/

void ConvexDecomposer::Log(const char* const msg) {

}
