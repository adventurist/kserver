FROM jonathonf/manjaro:latest
# Initial update of package lists
ENV TZ="America/Toronto"
RUN pacman -Sy --noconfirm base-devel gcc cmake git libpqxx postgresql postgresql-libs flatbuffers
RUN git clone https://github.com/jtv/libpqxx && cd libpqxx && cmake . && make && make install
