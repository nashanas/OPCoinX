### Setup docker on Debian 8.1

Install notes: https://docs.docker.com/install/linux/docker-ce/debian/#install-using-the-repository

Docker CLI: https://docs.docker.com/engine/reference/run/

Script that installs docker-ce: `setup_docker_debian.sh`

### Build&Run OPCX docker container

Build container: `sudo docker build --no-cache --tag opcx .`

Run container: `sudo docker run -d --name opcx.cont opcx`

Start container: `sudo docker start opcx.cont`

See if it is up: `sudo docker ps -a`

Shell in the container: `sudo docker exec -it opcx.cont /bin/bash`

Test RPC (shell inside container): `opcx-cli help` or `opcx-cli getinfo`

Stop container: `sudo docker stop opcx.cont`

Delete container: `sudo docker rm opcx.cont`