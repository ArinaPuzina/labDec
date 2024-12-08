#include <string>
#include <regex>
#include <iostream>
#include <fstream>
using namespace std;
// Базовый интерфейс
class CodeTransformer {
public:
    virtual string transform(const string& code) const = 0;
    virtual ~CodeTransformer() = default;
};

// Конкретный компонент
class PlainCodeTransformer : public CodeTransformer {
public:
    string transform(const string& code) const override {
        return "<pre><code>" + code + "</code></pre>";
    }
};

// Базовый декоратор
class CodeDecorator : public CodeTransformer {
protected:
    const CodeTransformer* component;

public:
    CodeDecorator(const CodeTransformer* comp) : component(comp) {}
    string transform(const string& code) const override {
        return component->transform(code);
    }
    virtual ~CodeDecorator() = default;
};

// Конкретные декораторы
class KeywordHighlighter : public CodeDecorator {
public:
    KeywordHighlighter(const CodeTransformer* comp) : CodeDecorator(comp) {}
    string transform(const string& code) const override {
        string html = component->transform(code);
        return regex_replace(html, regex("\\b(int|return|if|else|for|while|class)\\b"), 
                                  "<span style='color: blue;'>$&</span>");
    }
};

class CommentHighlighter : public CodeDecorator {
public:
    CommentHighlighter(const CodeTransformer* comp) : CodeDecorator(comp) {}
    string transform(const string& code) const override {
        string html = component->transform(code);
        return regex_replace(html, regex("(\\/\\/.*?\\n|\\/\\*[\\s\\S]*?\\*\\/)"), 
                                  "<span style='color: green;'>$&</span>");
    }
};

class StringHighlighter : public CodeDecorator {
public:
    StringHighlighter(const CodeTransformer* comp) : CodeDecorator(comp) {}
    string transform(const string& code) const override {
        string html = component->transform(code);
        return regex_replace(html, regex("(\".*?\"|'.*?')"), 
                                  "<span style='color: orange;'>$&</span>");
    }
};

// Использование декораторов
int main() {
    string code = R"(int main() {
    // Это комментарий
    string text = "Hello, world!";
    if (text == "Hello, world!") {
        return 0;
    }
})";

    CodeTransformer* base = new PlainCodeTransformer();
    CodeTransformer* withStrings = new StringHighlighter(base);
    CodeTransformer* withKeywords = new KeywordHighlighter(withStrings);
    CodeTransformer* withComments = new CommentHighlighter(withKeywords);
    

    // Преобразуем код в HTML
    string html = "<html><body>" + withComments->transform(code) + "</body></html>";

    // Записываем HTML в файл
    ofstream file("highlighted_code.html");
    if (file.is_open()) {
        file << html;
        file.close();
        cout << "HTML файл создан: highlighted_code.html" << endl;
    } else {
        cerr << "Ошибка: не удалось открыть файл для записи." << endl;
    }

    // Удаляем декораторы
    delete withStrings;
    delete withComments;
    delete withKeywords;
    delete base;

    return 0;
}