COMPILER = g++
TARGET = ses
ECOMPILED = simClasses.o
CPPFLAGS = -Wall -Wextra -Werror -g

asm: $(TARGET)

$(TARGET): $(ECOMPILED) main.cpp
	$(COMPILER) $(CPPFLAGS) -o $(TARGET) $(ECOMPILED) main.cpp

simClasses.o: simClasses.h simClasses.cpp
	$(COMPILER) $(CPPFLAGS) -c simClasses.cpp

clean:
	rm -f $(TARGET)
	rm -f $(ECOMPILED)