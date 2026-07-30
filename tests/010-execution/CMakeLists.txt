add_executable(a3d-runner-execution-tests
		RunnerExecutionTests.cc)

target_link_libraries(a3d-runner-execution-tests
		PRIVATE
		a3d::avara3d)

set(RUNNER_EXECUTION_TESTS
		first-update-zero-delta
		elapsed-time-progression
		delta-time-progression
		update-index-progression
		independent-runner-clocks
		simulation-config-defaults
		simulation-config-validation
		first-update-zero-simulation-ticks
		variable-step-scale-cap-discard
		fixed-step-fractional-accumulator
		fixed-step-catch-up-overflow
		pause-simulation-preconditions
		paused-update-pipeline
		pause-clears-fixed-accumulator
		pause-callback-boundaries
		pause-during-catch-up
		resume-boundary
		requested-tick-preconditions
		requested-tick-fixed-execution
		requested-tick-command-order
		requested-tick-snapshot-deferral
		requested-tick-stop-interruption
		requested-tick-resume-interruption
		stop-between-updates
		stop-during-input-update
		stop-during-runner-update
		stop-during-simulation-tick
		input-callback-order
		no-input-context
		application-runner-access
		simulation-callback-order-physics
		simulation-command-boundaries
		physics-command-boundaries
		render-command-no-valid-render
		neither-world-pipeline
		no-visual-completed-render-count
		scene-root-invariant
		physics-inventory-sampling
		physics-inventory-multiple-ticks
		physics-inventory-latest-tick
		physics-inventory-before-did-step
		bullet-exact-step
		bullet-invalid-step-delta
		fixed-step-repeatability
		fixed-step-120hz-long-run)

foreach(TEST_NAME IN LISTS RUNNER_EXECUTION_TESTS)
	add_test(
			NAME runner-execution.${TEST_NAME}
			COMMAND a3d-runner-execution-tests ${TEST_NAME})
endforeach()
