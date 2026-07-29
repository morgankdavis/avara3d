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
		simulation-configuration-defaults
		simulation-configuration-validation
		first-update-zero-simulation-ticks
		frame-driven-scale-cap-discard
		fixed-step-fractional-accumulator
		fixed-step-catch-up-overflow
		stop-between-updates
		stop-during-runner-update
		stop-during-simulation-tick
		simulation-callback-order-physics
		neither-world-pipeline
		no-visual-completed-render-count
		render-frame-simulation-metadata
		scene-root-invariant
		physics-inventory-sampling
		physics-inventory-multiple-ticks
		physics-inventory-latest-tick
		physics-inventory-before-did-step
		bullet-exact-step
		bullet-invalid-step-delta
		fixed-step-repeatability
		fixed-step-120hz-long-run)

foreach(TEST_NAME IN LISTS RUNNER_HOST_CLOCK_TESTS)
	add_test(
			NAME runner-host-clock.${TEST_NAME}
			COMMAND a3d-runner-host-clock-tests ${TEST_NAME})
endforeach()
