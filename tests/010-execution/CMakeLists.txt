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
		first-update-zero-simulation-steps
		simulation-fractional-accumulator
		simulation-catch-up-overflow
		pause-simulation-preconditions
		paused-update-pipeline
		pause-clears-fixed-accumulator
		pause-host-update-boundary
		pause-during-catch-up
		resume-boundary
		requested-step-preconditions
		requested-step-fixed-execution
		requested-step-command-order
		requested-step-snapshot-deferral
		requested-step-stop-interruption
		requested-step-resume-interruption
		stop-between-updates
		stop-during-host-update
		stop-during-runner-callback
		stop-during-simulation-step
		input-before-host-update
		no-input-context
		application-runner-access
		simulation-callback-order-physics
		scene-command-boundaries
		render-command-no-valid-render
		render-command-adapter-order
		neither-world-pipeline
		no-visual-completed-render-count
		scene-root-invariant
		physics-inventory-sampling
		physics-inventory-multiple-steps
		physics-inventory-latest-step
		physics-inventory-before-did-step
		bullet-exact-step
		bullet-invalid-step-delta
		simulation-repeatability
		simulation-120hz-long-run)

foreach(TEST_NAME IN LISTS RUNNER_EXECUTION_TESTS)
	add_test(
			NAME runner-execution.${TEST_NAME}
			COMMAND a3d-runner-execution-tests ${TEST_NAME})
endforeach()
