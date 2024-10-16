#include <Windows.h>

// нет дочернего окна но глобальная переменная остается

// Прототип функции окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND hMainWindow = NULL; // Главное окно
HWND hPopupWindow = NULL; // Всплывающее окно
HWND hChildWindow = NULL; // Дочернее окно

//функция изменения размера дочернего окна
void ChangingSizeChildWindow(HWND parentW) {
    // Получаем размеры клиентской области родительского окна
    RECT rect;
    GetClientRect(parentW, &rect);
    // Пересчитываем размеры дочернего окна
    MoveWindow(hChildWindow, 30, 30, rect.right - 60, rect.bottom - 60, TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    LPCTSTR szClass = TEXT("sdfghjkllkjhgf"); // Имя класса окна

    // Регистрация класса окна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc; // Указывается обработчик сообщений для главного окна
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Заливка окна
    wc.style = CS_DBLCLKS; // Отправляет сообщение о двойном щелчке в оконную процедуру
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Курсор

    // Регистрация класса
    if (!::RegisterClass(&wc)) {
        return -1;
    }

    // Создание главного окна
    hMainWindow = ::CreateWindow(szClass, "Главное окно", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!hMainWindow) {
        return -1; // Если окно не удалось создать, то возвращает ошибку
    }

    // Создание временного окна
    hPopupWindow = ::CreateWindow(szClass, "Временное окно", WS_POPUP | WS_CAPTION | 
        WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX, CW_USEDEFAULT, 
        CW_USEDEFAULT, 400, 300, hMainWindow, NULL, hInstance, NULL);
    if (!hPopupWindow) {
        return -1; // Если окно не удалось создать, то возвращает ошибку
    }

    // Создание дочернего окна
    hChildWindow = ::CreateWindow(szClass, "Дочернее окно", WS_CHILDWINDOW | WS_VISIBLE 
        | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hMainWindow, NULL, 
        hInstance, NULL);
    if (!hChildWindow) {
        return -1; // Если окно не удалось создать, то возвращает ошибку
    }
    ChangingSizeChildWindow(hMainWindow);

    // Показываем главное окно
    ::ShowWindow(hMainWindow, nCmdShow);

    MSG msg;
    // Цикл обработки сообщений
    while (::GetMessage(&msg, NULL, 0, 0)) {
        ::DispatchMessage(&msg); 
    }
    return 0;
}

// Обработчик сообщений для окон
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hChildWindow, &ps);

        // Заливаем окно цветом
        HBRUSH brush = CreateSolidBrush(RGB(90, 20, 205));
        FillRect(hdc, &ps.rcPaint, brush);
        DeleteObject(brush); // Освобождаем ресурсы

        EndPaint(hChildWindow, &ps);
        break;

    }

    case WM_LBUTTONDOWN: // Обработка сообщ. нажатия левой кнопки мыши
        if (IsWindow(hChildWindow)) {
            // При клике меняется родитель дочернего окна
            if (GetParent(hChildWindow) == hPopupWindow) {
                SetParent(hChildWindow, hMainWindow); // Перемещаем окно к главному 
                ChangingSizeChildWindow(hMainWindow);
            }
            else {
                SetParent(hChildWindow, hPopupWindow); // Перемещаем к временному
                ChangingSizeChildWindow(hPopupWindow);
            }
            return 0;
        }
        

    case WM_LBUTTONDBLCLK: // Обработка сообщ. двойного клика мыши
        //изменяем стиль временного окна (Topmost/Not Topmost)
        // Получаем текущий стиль окна
        if (hWnd == hPopupWindow) {
            LONG_PTR exStyle = GetWindowLongPtr(hPopupWindow, GWL_EXSTYLE); 
            SetWindowPos(hPopupWindow, (exStyle & WS_EX_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST,
            0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            // переменная exStyle получает расширенные стили временного окна
            // с помощью & проверяется установлен ли флаг WS_EX_TOPMOST, 
            // а не cравниваются переменная exStyle и WS_EX_TOPMOST. 
            // 
            //Поскольку используются флаги SWP_NOMOVE | SWP_NOSIZE, окно не будет перемещено 
            //и его размер не изменится.
        }
        return 0;

    case WM_DESTROY: // Закрытие окна
        if (hWnd == hChildWindow) {
            //DestroyWindow(hChildWindow);
            hChildWindow = NULL; // Обновляем переменную
        }
        if (hWnd == hMainWindow) {
            ::PostQuitMessage(0); 
        }
        return 0;

    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam); // Обработка других сообщений
    }
}