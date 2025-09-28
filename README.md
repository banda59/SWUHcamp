# SWUHcamp
CRUD

![](/images/1.png)
구현한 CRUD에 대한 캡쳐본을 첨부해보도록 하겠다.

![](/images/2.png)
```python
curl -sX POST http://127.0.0.1:8000/items -H "Content-Type: application/json" -d '{"title":"a"}'
curl -s http://127.0.0.1:8000/items
curl -s http://127.0.0.1:8000/items/1
curl -sX PUT http://127.0.0.1:8000/items/1 -H "Content-Type: application/json" -d '{"title":"b","done":true}'
curl -sX DELETE http://127.0.0.1:8000/items/1
```

![](/images/3.png)
```python
curl -s http://127.0.0.1:8000/items/1 -i
```
id=1인 아이템이 존재하지 않아서 FastAPI가 404를 반환하는 상황을 캡쳐했다. "detail":"Not Found"}는 의도한 예외 처리(raise HTTPException(404))가 정상 동작한다는 증거다.  


![](/images/4.png)
```python
curl -s http://127.0.0.1:8000/items
```
현재 DB(테이블 t)에 레코드가 하나도 없다는 뜻이고, 의도적으로 /items/1을 삭제해서 비어있는 상태다.  

![](/images/4.png)
