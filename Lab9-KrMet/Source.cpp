#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <cstring>
#include "Const.h"

using namespace std;

class Functions {
private:
public:
	// сдвиг (1)
	void shift1(string& str) {
		char a = str[0];
		for (int i = 0; i < str.size() - 1; i++) {
			str[i] = str[i + 1];
		}
		str[str.size() - 1] = a;
	}
	// сдвиг (2)
	void shift2(string& str) {
		char a = str[0], b = str[1];
		for (int i = 0; i < str.size() - 2; i++) {
			str[i] = str[i + 2];
		}
		str[str.size() - 2] = a;
		str[str.size() - 1] = b;
	}
	string xor_bin(string a, string b) {
		if (a.size() != b.size()) return "0";
		string rec = "";
		for (int i = 0; i < a.size(); i++) {
			rec += (char)((((a[i] - 48) + (b[i] - 48))%2) + 48);
		}
		return rec;
	}
	string E_expend(string str) {
		string rec = "";
		for (int i = 0; i < 48; i++) {
			rec += str[E_[i] - 1];
		}
		cout << "E = " << rec << endl;

		return rec;
	}
	string Feistel_SBOX(string str, int num) {
		int n = (str[0] - 48) * 2 + (str[5] - 48); //Строка в таблице
		int m = (str[1] - 48) * 8 + (str[2] - 48) * 4 + (str[3] - 48) * 2 + (str[4] - 48); //Столбец в таблице
		int number = SBox[num][n][m];
		string rec = "";
		while (number > 0) { //перевод в бинарный код числа таблицы
			rec = (char)(number % 2 + 48) + rec;
			number /= 2;
		}
		while (rec.size() < 4) {
			rec = "0" + rec;
		}
		return rec;
	}
	string getPT(string str) {
		string rec = "";
		for (int i = 0; i < 32; i++) {
			rec += str[P_transform[i] - 1];
		}
		return rec;
	}
	string Feistel(string R, string K) {
		string res = "", rec = "";
		// E-расширение строки Ri-1 до 48 бит
		string ER = E_expend(R);
		// E (Ri-1) и подключ Ki длиной 48 бит подвергаются операции XOR
		res = xor_bin(ER, K);
		// Равномерно разделить на 8 групп и пройти через 8 разных S-блоков для преобразования 6-4
		for (int i = 0; i < 8; i++) {
			string sub = res.substr(i * 6, 6);
			string sub_m = Feistel_SBOX(sub, i);
			// Подключаемся последовательно, чтобы получить строку длиной 32 бита
			rec += sub_m;
		}
		cout << "substring length: " << rec.size() << endl;

		// P - перестановка
		return getPT(rec);
	}
	string getIP1(string str) {
		string rec = "";
		for (int i = 0; i < 64; i++) {
			rec += str[IP_1[i] - 1];
		}
		return rec;
	}
};

class DES {
private:
	string number_key, number_text, C_str, D_str, L_str, R_str;
	vector<string> Key_steps;
	vector<char> Alfabet = { 'А', 'Б', 'В', 'Г', 'Д', 'Е', 'Ж', 'З', 'И', 'Й', 'К', 'Л', 'М', 'Н', 'О', 'П', 'Р', 'С', 'Т', 'У', 'Ф', 'Х', 'Ц', 'Ч', 'Ш', 'Щ', 'Ъ', 'Ы', 'Ь', 'Э', 'Ю', 'Я' };
	string Translate_En(char str[], int size_n) {
		vector<char>::iterator it;
		string buf_number = "";
		int buf;
		for (int i = 0; i < size_n; i++) {
			char buf_char = _toupper(str[i]);
			it = find(Alfabet.begin(), Alfabet.end(), buf_char);
			if (it == Alfabet.end()) return "0";
			buf = it - Alfabet.begin();
			if (buf < 16) {
				char buf_itoa[5];
				_itoa_s(buf, buf_itoa, 2);
				string itoa_str(buf_itoa);
				buf_number = buf_number + "1100";
				for (int j = 0; j < 4 - itoa_str.size(); j++) {
					buf_number.push_back('0');
				}
				buf_number = buf_number + itoa_str;
			}
			else {
				buf = buf % 16;
				buf_number = buf_number + "1101";
				char buf_itoa[5];
				_itoa_s(buf, buf_itoa, 2);
				string itoa_str(buf_itoa);
				for (int j = 0; j < 4 - itoa_str.size(); j++) {
					buf_number.push_back('0');
				}
				buf_number = buf_number + itoa_str;
			}
		}
		return buf_number;
	}
	void Key_Control() {
		string number_key_ = number_key;
		int pos = 7;
		for (int i = 0; i < 8; i++) {
			int buf_sum = 0;
			for (int j = 0; j < 7; j++) {
				buf_sum += number_key_[i * 8 + j] - 48;
			}
			if (buf_sum % 2 == 1) {
				number_key_.insert(number_key_.begin() + (i * 8) + 7, '1');
			}
			else 
				number_key_.insert((number_key_.begin() + (i * 8) + 7), '0');
		}
		number_key = number_key_;
	}
	string get_K_i(string str) { /*Ключ из 48 элементов по таблице 2*/
		string buf = "";
		for (int i = 0; i < 48; i++) {
			buf.push_back(str[CD_2[i] - 1]);
		}
		return buf;
	}
	string Step1() { /*Переработка ключа по C0 D0*/
		string buf = number_key;
		for (int i = 0; i < 56; i++) {
			buf[i] = number_key[CD_1[i] - 1];
		}
		return buf;
	}
	vector<string> Step2() { /*Выявление C и D*/
		string buf = number_key;
		for (int i = 7; i >= 0; i--) {
			buf.erase(8*i + 7, 1);
		}
		string C, D;
		C = buf.substr(0, 28);
		D = buf.substr(28);
		return { C, D };
	}
	void Step3() { /*Получение Ki*/
		string buf_C = C_str, buf_D = D_str;
		Functions F;
		cout << "Step3:" << endl;
		for (int i = 1; i <= 16; i++) {
			if (i == 1 || i == 2 || i == 9 || i == 16) {
				F.shift1(buf_C);
				F.shift1(buf_D);
			}
			else {
				F.shift2(buf_C);
				F.shift2(buf_D);
			}
			string temp = buf_C + buf_D;
			temp = get_K_i(temp);
			cout << "K" << i << " = " << temp << endl;
			Key_steps.push_back(temp);
		}
	}
	string Step4() { /*Переработка text*/
		string buf = number_text;
		for (int i = 0; i < 64; i++) {
			buf[i] = number_text[IP[i] - 1];
		}
		return buf;
	}
	vector<string> Step5() { /*Выявление L0 и R0*/
		string L, R;
		L = number_text.substr(0, 32);
		R = number_text.substr(32);
		return { L, R };
	}
	string Step6() { /*Получение Li и Ri*/
		string buf_L = L_str, buf_R = R_str, temp1, temp2;
		Functions F;
		cout << "Step6:" << endl;
		for (int i = 0; i < 15; i++) {
			cout << i+1 << ":" << endl;
			temp1 = buf_R;
			temp2 = F.xor_bin(buf_L, F.Feistel(buf_R, Key_steps[i]));
			buf_L = temp1;
			buf_R = temp2;
			cout << "L" << i << " = " << buf_L << endl;
			cout << "R" << i << " = " << buf_R << endl;
		}
		cout << 15 << ":" << endl;
		temp2 = buf_R;
		temp1 = F.xor_bin(buf_L, F.Feistel(buf_R, Key_steps[15]));
		buf_L = temp1;
		buf_R = temp2;
		cout << "L" << 15 << " = " << buf_L << endl;
		cout << "R" << 15 << " = " << buf_R << endl;
		L_str = buf_L;
		R_str = buf_R;
		return L_str + R_str;
	}
	string Step7(string cipher_) { /*Обратная перестановка по таблицу IP^-1*/
		Functions F;
		return F.getIP1(cipher_);
	}
public:
	void Encryption(char text[], char key[]) {
		int size_ = (sizeof(text) / sizeof(*text));
		if (size_ != 8) {
			cout << "Invalid text" << endl;
			return;
		}
		double size_1 = (sizeof(key) / sizeof(*key));
		size_ = size_1 - 1;
		if (size_ != 7) {
			cout << "Invalid key" << endl;
			return;
		}
		number_text = Translate_En(text, 8);
		if (number_text.size() < 4) {
			cout << "Invalid text" << endl;
			return;
		}
		cout << text << "(2) = " << number_text << endl;
		number_key = Translate_En(key, 7);
		if (number_key.size() < 4) {
			cout << "Invalid key" << endl;
			return;
		}
		cout << key << "(2) = " << number_key << endl;
		Key_Control();
		cout << "Key_control_bit = " << number_key << endl;
		number_key = Step1();
		cout << "Step_1: " << number_key << endl;
		vector<string> C_D = Step2();
		C_str = C_D[0];
		D_str = C_D[1];
		cout << "Step_2: " << endl << "C = " << C_str << endl << "D = " << D_str << endl;
		Step3();
		number_text = Step4();
		cout << "Step_4: " << number_text << endl;
		vector<string> L_R = Step5();
		L_str = L_R[0];
		R_str = L_R[1];
		cout << "Step_5: " << endl << "L = " << L_str << endl << "R = " << R_str << endl;
		string cipher = Step6();
		cout << "Str(L+R) = " << cipher << endl;
		cipher = Step7(cipher);
		cout << "Step_8:\nCipher = " << cipher << endl;
		cin.clear();
		cin.ignore(1000, '\n');
	}
};

int main() {
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	int task, exit = 1;
	string buf_in;
	string buf_in1;
	char text[9] = {};
	char key[8] = {};
	while (exit == 1) {
		DES Des;
		cout << "1.Encryption DES" << endl << "2.Exit" << endl << "Choose a way:" << endl;
		cin.clear();
		cin >> task;
		switch (task)
		{
		case 1:
			cout << "Input top text(8 bit):" << endl << "text = ";
			cin.clear();
			cin.ignore(1000, '\n');
			cin >> buf_in;
			strncpy_s(text, buf_in.c_str(), 8);
			cout << "Input top key(7 bit):" << endl << "key = ";
			cin.clear();
			cin.ignore(1000, '\n');
			cin >> buf_in1;
			strncpy_s(key, buf_in1.c_str(), 7);
			Des.Encryption(text, key);
			break;
		case 2:
			exit = 0;
			break;
		default:
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "This task does not exist" << endl;
			break;
		}
	}
	system("pause");
	return 0;
}
/*
щелкунов
аксения
*/