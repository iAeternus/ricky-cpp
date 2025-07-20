@echo off
curl -v http://127.0.0.1:8080/
curl -v http://127.0.0.1:8080/hello?name=Ricky
curl -v -X POST -d "test data" http://127.0.0.1:8080/submit
curl -v http://127.0.0.1:8080/static/
curl -v -L http://127.0.0.1:8080/old
