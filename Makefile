
APP = ftbrd

CFLAGS = -L. -lftd2xx

all: $(APP)

$(APP): main.c	
	$(CC) main.c -o $(APP) $(CFLAGS)
	
clean:
	rm -f $(APP)
