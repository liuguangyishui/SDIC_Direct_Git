
ALLFILE = main.o analysis_IR.o trance_fun.o out_put_fun.o debug_info.o \
	memory_manage.o memory_manage_reg.o memory_manage_data.o \
	tool_fun.o  

STD = -std=gnu++11

SDIC_GUN:	${ALLFILE}
		g++ ${ALLFILE} -o $@ ${STD}

main.o: main.cpp analysis_IR.h	
	g++ -c $< ${STD}

analysis_IR.o: analysis_IR.cpp analysis_IR.h key_data.h trance_fun.h debug_info.h	
	g++ -c $< ${STD}

trance_fun.o: trance_fun.cpp trance_fun.h out_put_fun.h memory_manage_reg.h memory_manage_data.h
	g++ -c $< ${STD}

out_put_fun.o: out_put_fun.cpp out_put_fun.h debug_info.h
	g++ -c $< ${STD} 

memory_manage.o: memory_manage.cpp memory_manage.h
	g++ -c $< ${STD}

memory_manage_reg.o: memory_manage_reg.cpp memory_manage_reg.h 
	g++ -c $< ${STD}

memory_manage_data.o: memory_manage_data.cpp memory_manage_data.h
	g++ -c $< ${STD}

debug_info.o : debug_info.cpp debug_info.h
	g++ -c $< ${STD}

tool_fun.o : tool_fun.cpp tool_fun.h
	g++ -c $< ${STD}

clean:
	rm -rf ${ALLFILE}
