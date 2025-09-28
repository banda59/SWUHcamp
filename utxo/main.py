from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel
from typing import List, Optional
from sqlalchemy import Column, Integer, String, Boolean, create_engine
from sqlalchemy.orm import sessionmaker, declarative_base, Session

DB_URL = "sqlite:///./app.db"
e = create_engine(DB_URL, connect_args={"check_same_thread": False})
S = sessionmaker(autocommit=False, autoflush=False, bind=e)
B = declarative_base()

class T(B):
    __tablename__ = "t"
    id = Column(Integer, primary_key=True, index=True)
    title = Column(String, index=True)
    done = Column(Boolean, default=False)

class I(BaseModel):
    title: str
    done: Optional[bool] = False

class O(BaseModel):
    id: int
    title: str
    done: bool
    class Config:
        from_attributes = True

def get_s():
    s = S()
    try:
        yield s
    finally:
        s.close()

B.metadata.create_all(bind=e)
app = FastAPI()

@app.post("/items", response_model=O)
def create_item(x: I, s: Session = Depends(get_s)):
    o = T(title=x.title, done=x.done or False)
    s.add(o)
    s.commit()
    s.refresh(o)
    return o

@app.get("/items", response_model=List[O])
def list_items(s: Session = Depends(get_s)):
    return s.query(T).all()

@app.get("/items/{i}", response_model=O)
def get_item(i: int, s: Session = Depends(get_s)):
    o = s.query(T).filter(T.id == i).first()
    if not o:
        raise HTTPException(404)
    return o

@app.put("/items/{i}", response_model=O)
def update_item(i: int, x: I, s: Session = Depends(get_s)):
    o = s.query(T).filter(T.id == i).first()
    if not o:
        raise HTTPException(404)
    o.title = x.title
    o.done = bool(x.done)
    s.commit()
    s.refresh(o)
    return o

@app.delete("/items/{i}")
def delete_item(i: int, s: Session = Depends(get_s)):
    o = s.query(T).filter(T.id == i).first()
    if not o:
        raise HTTPException(404)
    s.delete(o)
    s.commit()
    return {"ok": True}

