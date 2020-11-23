1. 创建index由catalog->buffer（表头数据更新），catalog->index（告知index新添文件（**格式1**）），index->buffer->index（该过程实现index对元组的读入（**目前未定格式2**）），index->buffer（输出一个文件块txt（**目前未定格式**3））
2. 删除index直接由catalog->buffer，不经过index（因为只需要更改表头文件，删除对应index的txt文件即可）
3. 查询index：
   1. 由record->index查询的是哪个表的哪个属性，附带查询条件（**格式4**）
   2. index->buffer->index（**目前未定格式5**），index告知buffer取数据，然后传递给index进行下一步查询操作
   3. index->record一个数组文件（**格式6**），查询结果信息，然后由record完成对该信息的下一步操作。



格式1：

​	catalog传给index两个string，分别是表名称、属性名称

格式4：

