
function(print_all_variables)
	# https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script?answertab=trending#tab-top
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endfunction()
