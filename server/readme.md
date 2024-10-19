# fireVolt Server


## .env 파일 설정

```txt
PORT=3000
MONGO_URI=mongodb://localhost:27017/fireVolt
DB_NAME=redstar
```

## 프로젝트 설치

```bash
$ npm install
```

## How to Run the Project

디버깅 모드로 실행하려면 다음 명령을 실행하십시오.

```bash
$ npm run dev
```

프로덕션 모드로 실행하려면 다음 명령을 실행하십시오.

```bash
$ npm run start
```

pm2 를 사용하여 프로덕션 모드로 실행하려면 다음 명령을 실행하십시오.

```bash
$ npm run pm2
```

vscode 디버깅을 사용하여 디버깅 모드로 실행하려면 다음 명령을 실행하십시오.

디랙토리 구조는 workspaceFolder/server 이어야 합니다.  

```json
{
            "name": "Degbug FireBoltServer",
            "type": "node",
            "request": "launch",
            "runtimeExecutable": "npm",
            "runtimeArgs": ["run", "dev"],
            "cwd": "${workspaceFolder}/server",
            "console": "integratedTerminal",
            "env": {
                "NODE_ENV": "dev"
            }
        }
```


## API 명세

# Log Management API Documentation

## Version: 0.0.1

### Base URL
```
/api/logs/...
```

### Common Middleware Used
- **CORS Policy**: Applied to allow cross-origin requests.
- **Body Parsing**:
  - `express.raw({ limit: '500kb' })` for `application/octet-stream`
  - `express.json({ limit: '500kb' })` for `application/json`
  - `express.text({ limit: '500kb' })` for `text/plain`
  - `express.urlencoded({ extended: true })` for `application/x-www-form-urlencoded`

---

## Endpoints

### 1. Get Log System Info
**Endpoint**: `/api/logs/`

**Method**: `GET`

**Description**: Returns the version information of the log system.

**Response**:
```json
{
  "r": "ok",
  "info": "log system v 0.0.1"
}
```

---

### 2. Insert Log
**Endpoint**: `/api/logs/insert`

**Method**: `POST`

**Description**: Inserts a new log entry.

**Request Body** (example):
```json
{
  "title": "Log Title",
  "content": "Log Content",
  "tags": ["tag1", "tag2"]
}
```

**Automatic Fields**:
- `rdate`: Current date and time (format: `YYYY-MM-DD HH:mm:ss`).
- `address`: Client's IP address.

**Response**:
```json
{
  "r": "ok",
  "data": {
    "sid": "<inserted log ID>",
    "rdate": "<insertion date>"
  }
}
```

**Error Response**:
```json
{
  "r": "fail",
  "info": "<error details>"
}
```

---

### 3. List Logs
**Endpoint**: `/api/logs/list`

**Method**: `POST`

**Description**: Returns a list of logs based on the provided query.

**Request Body** (example):
```json
{
  "page": 1,
  "unit": 10,
  "option": {
    "tags": "tag1"
  }
}
```

**Response**:
```json
{
  "r": "ok",
  "list": [
    {
      "_id": "<log ID>",
      "title": "Log Title",
      "rdate": "<record date>",
      "tags": ["tag1", "tag2"]
    }
  ],
  "count": <total logs>,
  "lastPage": <last page number>
}
```

**Error Response**:
```json
{
  "r": "fail",
  "data": "<error details>"
}
```

---

### 4. Get Log Detail
**Endpoint**: `/api/logs/detail`

**Method**: `POST`

**Description**: Retrieves the details of a specific log.

**Request Body** (example):
```json
{
  "id": "<log ID>"
}
```

**Response**:
```json
{
  "r": "ok",
  "data": {
    "_id": "<log ID>",
    "title": "Log Title",
    "content": "Log Content",
    "rdate": "<record date>",
    "tags": ["tag1", "tag2"]
  }
}
```

**Error Response**:
```json
{
  "r": "fail",
  "data": "<error details>"
}
```

---

### 5. Delete Log
**Endpoint**: `/api/logs/delete`

**Method**: `POST`

**Description**: Deletes one or more logs based on the provided query.

**Request Body** (example):
```json
{
  "_id": "<log ID>"
}
```

**Response**:
```json
{
  "r": "ok",
  "data": "delete count : <number of deleted logs>"
}
```

**Error Response**:
```json
{
  "r": "fail",
  "info": "<error details>"
}
```

---

### 6. Update Log
**Endpoint**: `/api/logs/update`

**Method**: `POST`

**Description**: Updates a specific log based on the provided ID and data.

**Request Body** (example):
```json
{
  "id": "<log ID>",
  "update": {
    "title": "Updated Title",
    "content": "Updated Content"
  }
}
```

**Response**:
```json
{
  "r": "ok",
  "data": "update count : <number of updated logs>"
}
```

**Error Response**:
```json
{
  "r": "fail",
  "info": "<error details>"
}
```

**Notes**:
- The `id` must be a valid MongoDB ObjectId.
- If `id` is invalid, an appropriate error message will be returned.


