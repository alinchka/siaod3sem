#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

using namespace std;

// структура товара
struct product {
    int code = 0;   // Код товара - шестизначное число
    string name;    // Название товара
    int cost;       // Стоимость товара
};

void printProduct(product currProduct);
int getRandomNumber(int min, int max);

class HashTable {

    product* table;
    int size;
    int offset;
    int countOfRecords;

    // Нахождение смещения, которое будет взаимно простое с размером таблицы
    int findOffset(int count) {
        for (int i = 3; i < count; i++) {
            if (gcd(i, count) == 1) {
                return i;
            }
        }
        return 2;
    }

    // Хеш-функция, возвращающая начальную позицию для элемента
    int hashFunc(int code, int currSize) {
        return code % currSize;
    }

    // Алгоритм Евклида для нахождения НОД, чтобы определить смещение при линейной пробации
    int gcd(int a, int b) {
        int t;
        while (b != 0) {
            t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    // Метод рехеширования таблицы
    void rehash() {
        // Новый размер, увеличенный в два раза
        int newSize = this->size * 2;
        // Новая таблица с размером в два раза больше
        auto* newTable = new product[newSize];
        // Новое смещение, которое так же является взаимно обратным числом с размером таблицы
        int newOffset = findOffset(newSize);

        // Бежим по всем элементам старой таблицы
        for (int i = 0; i < this->size; i++) {
            // Если текущая запись не пустая, то будем ее вставлять в новую таблицу
            if (this->table[i].code > 0) {
                // Находим начальную позицию в новой таблице
                int j = hashFunc(this->table[i].code, newSize);
                // Ищем первый свободный слот
                while (newTable[j].code > 0) {
                    j = (j + newOffset) % newSize;
                }
                // Вставляем запись из старой таблицы в новую
                newTable[j] = this->table[i];
            }
        }

        // Заменяем размер таблицы на новый
        this->size = newSize;
        // Заменяем смещение на новое
        this->offset = newOffset;
        // Ставим новую таблицу взамен старой
        this->table = newTable;
    }

public:
    // Конструктор, получает на вход размер входных данных
    HashTable(int size) {
        table = new product[size];                // Устанавливаем размер таблицы в два раза больше входных данных
        this->size = size;                        // Устанавливаем значения поля размера таблицы
        this->offset = findOffset(size);   // Устанавливаем значения для смещения
        this->countOfRecords = 0;                // Устанавливаем значение количества записей изначально в ноль
    }

    // Метод вставки нового элемента в хеш-таблицу
    void insertToTable(product currProduct) {
        // Находим начальную позицию
        int i = hashFunc(currProduct.code, this->size);
        // Ищем первый свободный слот
        while (this->table[i].code > 0) {
            i = (i + this->offset) % this->size;
        }
        // Вставляем запись
        this->table[i] = currProduct;
        this->countOfRecords++;
        // Если заполнено больше 75% таблицы, увеличиваем ее размер в два раза
        if (this->countOfRecords > this->size * 3 / 4) {
            this->rehash();
        }
    }

    // Метод поиска записи в таблице по ключу
    int findByKey(int key, product& currProduct) {
        // Находим начальную позицию
        int i = hashFunc(key, this->size);
        // Количество пройденных записей
        int count = 0;

        // Пока не найдена нужная запись или не пройдены все ячейки
        while (this->table[i].code != key) {
            i = (i + this->offset) % this->size;
            count++;
            if (count > this->size) {
                // В таблице нет такого элемента
                return -1;
            }
        }
        // Возвращаем найденную запись
        currProduct = this->table[i];
        // Поиск выполнен успешно
        return 0;
    }

    // Метод удаления записи по ключу
    int deleteByKey(int key) {
        // Находим начальную позицию
        int i = hashFunc(key, this->size);
        // Количество пройденных записей
        int count = 0;

        // Пока не найдена нужная запись или не пройдены все ячейки
        while (this->table[i].code != key) {
            i = (i + this->offset) % this->size;
            count++;
            if (count > this->size) {
                // В таблице нет такого элемента
                return -1;
            }
        }

        // "Удаляем" элемент
        this->table[i].code = -1;
        this->countOfRecords--;
        // Удаление выполнено успешно
        return 0;
    }

    // Вывод таблицы
    void printTable() {
        cout << "Размер таблицы: " << this->size << endl;
        cout << "Смещение: " << this->offset << endl;
        cout << "Количество заполненных записей: " << this->countOfRecords << endl;

        for (int i = 0; i < this->size; i++) {
            // Незаполненная ячейка
            if (this->table[i].code <= 0) {
                cout << "[" << i << "] пустая" << endl;
            }
            else {
                // Заполненная ячейка
                cout << "[" << i << "] = ";
                printProduct(this->table[i]);
            }
        }
    }

    // Заполняем таблицу рандомными значениями
    void fillTable(int count) {
        // Список возможных товаров
        string arrayOfNames[7]{ "BMW X5", "LR Range Rover", "Lada Vesta", "Toyota RAV4", "Lexus LX570", "Volkswagen Golf",
                               "Audi 80" };

        for (int i = 0; i < count; i++) {
            // Создаем новую запись
            product newProduct;
            newProduct.code = getRandomNumber(pow(10, 5), pow(10, 6) - 1);
            newProduct.cost = getRandomNumber(pow(10, 5), 2 * pow(10, 7));
            newProduct.name = arrayOfNames[getRandomNumber(0, 6)];

            // Добавляем ее таблицу
            this->insertToTable(newProduct);
        }
    }
};

// Вывод информации о конкретной записи
void printProduct(product currProduct) {
    cout << "Код: " << currProduct.code << "    Имя: " << currProduct.name << "    Цена: " << currProduct.cost << endl;
}

// Функция получения рандомного числа в пределах
int getRandomNumber(int min, int max) {
    // Получить случайное число - формула
    int num = min + rand() % (max - min + 1);

    return num;
}


int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    // Для рандомного заполнения таблицы
    srand(time(0));
    // Пока флаг - истина, продолжаем показ меню
    bool isGoing = true;
    // Переменная для выбранного номера меню
    int numOfChoice;
    cout << "Введите размер таблицы: ";
    cin >> numOfChoice;
    // Создаем хеш-таблицу
    HashTable hashTable(numOfChoice);

    while (isGoing) {
        cout << "МЕНЮ" << endl;
        cout << "1. Заполнить запись и вставить ее в таблицу" << endl;
        cout << "2. Удалить запись из таблицы по ключу" << endl;
        cout << "3. Найти запись по ключу" << endl;
        cout << "4. Вывести таблицу" << endl;
        cout << "5. Заполнить таблицу случайными значениями" << endl;
        cout << "0. Выйти из программы" << endl;
        cout << "Номер команды: ";
        cin >> numOfChoice;

        switch (numOfChoice) {
        case 1: {
            product newProduct;
            cout << "Введите код товара (шестизначное число): ";
            cin >> newProduct.code;
            if (newProduct.code < pow(10, 5) || newProduct.code >= pow(10, 6)) {
                cout << "Вы ввели недопустимый код товара" << endl;
                break;
            }
            cout << "Введите имя товара: ";
            cin.ignore(1);
            getline(cin, newProduct.name);
            cout << "Введите стоимость товара: ";
            cin >> newProduct.cost;

            hashTable.insertToTable(newProduct);
            break;
        }

        case 2: {
            int key;
            cout << "Введите ключ: ";
            cin >> key;

            if (hashTable.deleteByKey(key) == 0) {
                cout << "Удаление выполнено успешно" << endl;
            }
            else {
                cout << "Элемента с таким ключом в таблице нет :(" << endl;
            }
            break;
        }

        case 3: {
            product targetProduct;
            int key;
            cout << "Введите ключ: ";
            cin >> key;

            if (hashTable.findByKey(key, targetProduct) == 0) {
                cout << "Найденная запись:" << endl;
                printProduct(targetProduct);
            }
            else {
                cout << "Элемента с таким ключом в таблице нет :(" << endl;
            }

            break;
        }

        case 4: {
            hashTable.printTable();
            break;
        }

        case 5: {
            int count;
            cout << "Введите количество случайных значений: ";
            cin >> count;
            hashTable.fillTable(count);
            break;
        }

        case 0: {
            isGoing = false;
            break;
        }

        default: {
            cout << "Неверный код команды :(" << endl;
        }
        }
    }
}
