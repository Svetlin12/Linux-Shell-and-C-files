## Task 8

Напишете скрипт, който приема два позиционни аргумента - имена на текстови файлове в CSV формат:

8,foo,bar,baz

2,quz,,foo

12,1,3,foo

3,foo,,

5,,bar,

7,,,

4,foo,bar,baz

Валидни са следните условия:
* CSV файловете представляват таблица, като всеки ред на таблицата е записан на отделен ред;
* на даден ред всяко поле (колона) е разделено от останалите със запетая;
* броят на полетата на всеки ред е константа;
* в полетата не може да присъства запетая, т.е., запетаята винаги е разделител между полета;
* ако във файла присъстват интервали, то това са данни от дадено поле;
* първото поле на всеки ред е число, което представлява идентификатор на реда (ID).

*Примерно извикване:* ./foo.sh a.csv b.csv

Скриптът трябва да чете a.csv и на неговата база да създава b.csv по следния начин: 
* някои редове във файла се различават само по колоната ID, и за тях казваме, че формират множество *A<sub>i</sub>*;
* за всяко такова множество *A<sub>i</sub>* да се оставя само един ред - този с най-малка стойност на ID-то;
* редовете, които не са членове в някое множество *A<sub>i</sub>* се записват в изходния файл без промяна.

[Solution](https://github.com/Svetlin12/Linux-Shell-and-C-files/blob/master/FMITasks/Task08-Solution.sh)