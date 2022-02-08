FROM ubuntu

RUN apt update; apt install -y build-essential libtool clang vim git curl

RUN cd /home; git clone https://github.com/JoeDog/siege.git; cd siege; utils/bootstrap; ./configure; make; make install