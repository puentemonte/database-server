# Database Server in C

From [this](https://youtu.be/qBvz5BqUOH8?si=NuJ3WMcAsbLsaGWe) video.

Listening to [this](https://youtu.be/2EYUKW2o-5Q?si=7261jkQT0j76CoPi) while programming which was nice, will keep a playlist of what I listen to.

Basic structure of DB consists of a (not quite) Binary Tree:
* Nodes
* Leaves

Essentially the DB is going to work as a directory where a Node is a intermediate data structure storing pointers to other nodes that are related (as in parent folder or folders inside a folder). For leaves, they are going to serve as data elements.

Our Tree is a union-type structure. This allows us to only hold one of it's two possible values: either a Node or a Leaf. Both Nodes and Leaves are structs. 

To better illustrate the overall structure of the DB:

```
/ -> RootNode (Node)
    /User/ (Node)
        /User/Login/ (Node) -> Erik (Leaf) -> ...
```      

- Why do an in-memory DB?
Fast read & write.
DB between real DB and client (for example sessions in web dev).

Focuses on CPU optimization. Not on Network optimization.

So, the DB is going to be called Cache23 (since the original is called Cache22).

Something useful when debugging C code:
```
strace -f ./executable
```

TODO:
- [x] Finish Binary Tree impl.
- [ ] Create Redis database server (currently working on this!)
- [ ] Create faster database server
- [ ] Implement print funcs.
- [ ] Implement search algo.
