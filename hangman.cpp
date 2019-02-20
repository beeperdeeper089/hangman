#include <iostream>
#include <vector>
#include <string>

#include <chrono>
#include <thread>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#endif

char alphabet[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

void clearConsole()
{
#ifdef WIN32
	system("cls");
#else
	system("clear");
#endif
}

class Hangman
{
public:
	Hangman();
	~Hangman() = default;

	void setWord(const std::string& word) { m_guessWord = word; }

	void printMessage(std::string message, bool topBorder, bool bottomBorder) const;
	void printHangman() const;
	void printRemainingLetters() const;
	void printWord();

	void takeGuess(char letter);
	unsigned int guessesLeft() const;

	bool checkWin();

private:
	std::vector<char> m_guesses;

	std::string m_guessWord = "SHREK";

	std::size_t m_consoleWidth;

	void centreAlignString(std::string& message) const;
};

Hangman::Hangman()
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	m_consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	if (m_consoleWidth % 2 == 0)
		--m_consoleWidth;
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	m_consoleWidth = w.ws_col;
#endif
}

void Hangman::printMessage(std::string message, bool topBorder, bool bottomBorder) const
{
	if (topBorder)
	{
		std::string output;

		output.append("+");
		for (std::size_t i = 0; i < m_consoleWidth - 3; ++i)
			output.append("-");
		output.append("+");

		centreAlignString(output);

		std::cout << output << std::endl;
	}

	centreAlignString(message);
	std::cout << message << std::endl;

	if (bottomBorder)
	{
		std::string output;

		output.append("+");
		for (std::size_t i = 0; i < m_consoleWidth - 3; ++i)
			output.append("-");
		output.append("+");

		centreAlignString(output);

		std::cout << output << std::endl;
	}
}

void Hangman::printHangman() const
{
	int guessCount = 0;

	for (const char letter : m_guesses)
	{
		if (m_guessWord.find(letter))
			++guessCount;
	}

	if (guessCount >= 1)	printMessage("|", true, false);
	else					printMessage("", true, false);

	if (guessCount >= 2)	printMessage("|", false, false);
	else					printMessage("", false, false);

	if (guessCount >= 3)	printMessage("O", false, false);
	else					printMessage("", false, false);

	if (guessCount == 4)	printMessage("/  ", false, false);

	if (guessCount == 5)	printMessage("/| ", false, false);

	if (guessCount >= 6)	printMessage("/|\\", false, false);
	else					printMessage("", false, false);

	if (guessCount >= 7)	printMessage("|", false, false);
	else					printMessage("", false, false);

	if (guessCount == 8)	printMessage("/", false, false);

	if (guessCount >= 9)	printMessage("/ \\", false, false);
	else					printMessage("", false, false);
}

void Hangman::printRemainingLetters() const
{
	printMessage("AVAILABLE LETTERS", true, true);

	{
		std::string output = " ";
		for (std::size_t i = 0; i < sizeof(alphabet) / sizeof(char) / 2; ++i)
		{
			if (std::find(m_guesses.begin(), m_guesses.end(), alphabet[i]) == m_guesses.end())
			{
				output += alphabet[i];
				output += " ";
			}
			else
			{
				output += " ";
				output += " ";
			}
		}

		printMessage(output, false, false);
	}

	{
		std::string output = " ";
		for (std::size_t i = sizeof(alphabet) / sizeof(char) / 2; i < sizeof(alphabet) / sizeof(char); ++i)
		{
			if (std::find(m_guesses.begin(), m_guesses.end(), alphabet[i]) == m_guesses.end())
			{
				output += alphabet[i];
				output += " ";
			}
			else
			{
				output += " ";
				output += " ";
			}
		}

		printMessage(output, false, true);
	}
}

void Hangman::printWord()
{
	std::string output;

	for (std::size_t i = 0; i < m_guessWord.length(); ++i)
	{
		if (std::find(m_guesses.begin(), m_guesses.end(), m_guessWord[i]) != m_guesses.end())
		{
			output += m_guessWord[i];
			output += " ";
		}
		else
		{
			output += "_";
			if (i < m_guessWord.length() - 1)
				output += " ";
		}
	}

	printMessage(output, false, true);
}

void Hangman::takeGuess(char letter)
{
	if (std::find(m_guesses.begin(), m_guesses.end(), letter) == m_guesses.end())
		m_guesses.push_back(letter);
	else
	{
		printMessage("Oops! You've already used this letter!", false, false);
		std::this_thread::sleep_for(std::chrono::milliseconds(750));
	}
}

unsigned int Hangman::guessesLeft() const
{
	int incorrect = 0;

	for (const char letter : m_guesses)
	{
		if (m_guessWord.find(letter) == std::string::npos)
			++incorrect;
	}

	return 10 - incorrect;
}

bool Hangman::checkWin()
{
	std::string word;

	for (auto letter : m_guesses)
	{
		if (m_guessWord.find(letter) != std::string::npos)
			word += letter;
	}

	return word == m_guessWord;
}

void Hangman::centreAlignString(std::string& message) const
{
	bool front = true;
	for (std::size_t i = message.length(); i < m_consoleWidth; i++)
	{
		if (front)
		{
			message = " " + message;
		}
		else
		{
			message = message + " ";
		}
		front = !front;
	}
}

int main()
{
	Hangman hangman;

	hangman.printMessage("Welcome to Hangman!", true, false);
	hangman.printMessage("Made By Vyom Fadia!", false, false);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	hangman.printHangman();
	hangman.printRemainingLetters();

	while (hangman.guessesLeft() > 0)
	{
		clearConsole();

		hangman.printMessage("Welcome to Hangman!", true, false);
		hangman.printMessage("Made By Vyom Fadia!", false, false);

		hangman.printHangman();
		hangman.printRemainingLetters();
		hangman.printWord();

		hangman.printMessage("Guess the Word!", true, false);

		char response;
		std::cout << ">>";
		std::cin >> response;

		hangman.takeGuess(response);

		if (hangman.checkWin())
			break;
	}

	if (hangman.checkWin())
		hangman.printMessage("HOLY MOLY, YOU WON!", true, true);
	else
		hangman.printMessage("OH NOES! YOU LOST, GOOD LUCK NEXT TIME! :(", true, true);

	std::cout << "Press any key to exit...";
	std::cin.get();

	return 0;
}
