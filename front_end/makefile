install: build clean remove

build :
	docker build -t "front_end" .
run :
	docker run -d -it -rm -p 4000:3000 --name "fe" front_end
	
# running :
# 	docker run -fe


clean :	
	docker ps -a
	docker rm fe

remove :
	docker images
	docker rmi front_end