.PHONY: build dev stop clean backend frontend

build:
	cd backend/build && cmake .. && make

backend:
	cd backend/build && ./server

frontend:
	cd frontend && npm run dev

dev:
	@trap 'kill 0' EXIT; \
	(cd backend/build && ./server) & \
	(cd frontend && npm run dev) & \
	wait

clean:
	rm -rf backend/build/*
	rm -rf frontend/dist
	rm -rf frontend/node_modules/.vite

stop:
	-lsof -ti:8080 | xargs kill -9 2>/dev/null || true
	-lsof -ti:5173 | xargs kill -9 2>/dev/null || true