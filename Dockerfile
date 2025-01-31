FROM devkitpro/devkitarm

# DevkitARM dev container



# a lil update
RUN apt-get update

# a lil copy
COPY utils/bin/bannertool /usr/bin/
COPY utils/bin/makerom /usr/bin

# a lil executable maker
RUN chmod +x /usr/bin/bannertool
RUN chmod +x /usr/bin/makerom
