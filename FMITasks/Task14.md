## Task 14

Напишете shell script, който получава задължителен първи позиционен параметър - директория и незадължителен втори - име на файл.
Скриптът трябва да намира в подадената директория и нейните под-директории всички symlink-ове и да извежда (при подаден аргумент
файл - добавяйки към файла, а ако не е - на стандартния изход) за тях следната информация:
* ако destination-а съществува - *името на symlink-а -> името на destination-a*;
* броя на symlink-овете, чийто destination не съществува.

*Примерен изход:*

lbaz -> /foo/bar/baz

lqux -> ../../../qux

lquux -> /foo/quux

Broken symlinks: 34

[Solution](https://github.com/Svetlin12/Linux-Shell-and-C-files/blob/master/FMITasks/Task14-Solution.sh)
