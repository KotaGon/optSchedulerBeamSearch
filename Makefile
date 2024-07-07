COMPILER = clang++
CFLAGS   = -g -Wall -O3 -std=c++1z
LDFLAGS  = 
LIBS     = 
INCLUDE  = -I./include -I./
TARGET   = a.out
OBJDIR   = ./obj
INCDIR   = ./include
SOURCES  = $(wildcard *.cc)
HEADERS = $(wildcard $(INCDIR)/*.h $(INCDIR)/*hpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cc=.o))

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o: %.cc
	@[ -d $(OBJDIR) ]
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJECTS) : $(HEADERS)

all: clean $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
