## Task 40

Напишете програма на С, която да работи подобно на командата cut, реализирайки само следната функционалност:
* програмата трябва да чете текст от стандартния вход и да извежда избраната част от всеки ред на стандартния изход;
* ако първият параметър на програмата е низът -с, тогава вторият параметър е или едноцифрено число (от 1 до 9), или две
едноцифрени числа N и M (N <= M), разделени с тире (напр. 3-5). В този случай програмата трябва да изведе избраният/избраните
символи от реда: или само символа на указаната позиция, или няколко последователни символи на посочените позиции.
* ако първият параметър на програмата е низът -d, тогава вторият параметър е низ, от който е важен само първият символ;
той се използва като разделител между полетата на реда. Третият параметър трябва да бъде низът -f а четвъртият - или 
едноцифрено число (от 1 до 9), или две едноцифрени числа N и M (N <= M), разделени с тире (напр. 3-5). В този случай програмата
трябва да разглежда реда като съставен от няколко полета (може би празни), разделени от указания символ (първият символ от низа,
следващ параметъра -d), и да изведе или само указаното поле, или няколко последователни полета на указаните позиции, разделени
от същия разделител.
* ако някой ред няма достатъчно символи/полета, за него програмата трябва изведе каквото (докъдето) е възможно (или дори празен
ред).

[Solution](https://github.com/Svetlin12/Linux-Shell-and-C-files/blob/master/C/FMI/task40.c)