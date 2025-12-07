
#ifndef AVARA3D_PROFILER_H
#define AVARA3D_PROFILER_H

#include <chrono>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>

namespace a3d {

	class Profiler {

	public:

		enum class Tag {
			WholeFrame,
			EngineCpu,
			Physics,
			RenderSubmission,
			// Draw?
			Application
		};

//		static Profiler& Instance();

		Profiler() = default;
		Profiler(const Profiler&) = delete;
		Profiler& operator=(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;
		Profiler& operator=(Profiler&&) = delete;

		void add(Tag tag, std::chrono::nanoseconds ns);
		void add(std::string key, std::chrono::nanoseconds ns);

		std::chrono::nanoseconds time(Tag tag) const;
		std::chrono::nanoseconds time(std::string key) const;

		void reset();

	private:

//		Profiler() = default;
//		~Profiler() = default;

		using TaggedSample = 	std::tuple<Tag, std::chrono::nanoseconds>;
		using KeyedSample = 	std::tuple<std::string, std::chrono::nanoseconds>;

		std::set<TaggedSample>	_taggedSamples;
		std::set<KeyedSample> 	_keyedSamples;
	};
}

#endif //AVARA3D_PROFILER_H
