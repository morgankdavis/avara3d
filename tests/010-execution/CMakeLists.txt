add_executable(a3d-runner-host-clock-tests
		RunnerHostClockTests.cc)

target_link_libraries(a3d-runner-host-clock-tests
		PRIVATE
		a3d::avara3d)

set(RUNNER_HOST_CLOCK_TESTS
		first-update-zero-delta
		elapsed-time-progression
		delta-time-progression
		update-index-progression
		independent-runner-clocks
		stop-between-updates
		stop-during-runner-update
		stop-during-did-simulate
		runner-callback-order-physics
		first-zero-delta-reaches-physics
		neither-world-pipeline
		no-visual-completed-render-count
		scene-root-invariant
		physics-inventory-sampling
		physics-inventory-before-did-simulate)

foreach(TEST_NAME IN LISTS RUNNER_HOST_CLOCK_TESTS)
	add_test(
			NAME runner-host-clock.${TEST_NAME}
			COMMAND a3d-runner-host-clock-tests ${TEST_NAME})
endforeach()
