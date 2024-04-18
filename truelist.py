import getpass
from datetime import datetime, timezone
import sqlite3
import hashlib
import uuid

def create_users_table():
    conn = sqlite3.connect("users.db")
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS users
                 (username TEXT PRIMARY KEY, password TEXT)''')
    conn.commit()
    conn.close()

def create_tasks_table():
    conn = sqlite3.connect("tasks.db")
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS tasks
                 (id TEXT PRIMARY KEY, user_id TEXT, date TEXT, text TEXT)''')
    conn.commit()
    conn.close()

def register_user(username, password):
    hashed_password = hashlib.sha256(password.encode()).hexdigest()

    conn = sqlite3.connect("users.db")
    c = conn.cursor()

    try:
        c.execute("INSERT INTO users (username, password) VALUES (?, ?)", (username, hashed_password))
        conn.commit()
        print("Пользователь успешно зарегистрирован!")
    except sqlite3.IntegrityError:
        print("Пользователь с таким именем уже существует.")

    conn.close()

def authenticate_user(username, password):
    hashed_password = hashlib.sha256(password.encode()).hexdigest()

    conn = sqlite3.connect("users.db")
    c = conn.cursor()

    c.execute("SELECT * FROM users WHERE username = ? AND password = ?", (username, hashed_password))
    user = c.fetchone()

    if user:
        print("Аутентификация прошла успешно!")
        return user[0]  # Возвращаем ID пользователя
    else:
        print("Неверное имя пользователя или пароль.")
        return None

def add_task(user_id):
    task = input("Введите новое дело, которое предстоит выполнить: ")
    date = datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S')
    task_id = str(uuid.uuid4())
    write_to_journal(task_id, user_id, date, task)
    print("Дело успешно добавлено в ежедневник!")

def delete_task(task_id):
    conn = sqlite3.connect("tasks.db")
    c = conn.cursor()

    c.execute("DELETE FROM tasks WHERE id=?", (task_id,))
    if c.rowcount == 0:
        print("Дело с таким ID не найдено.")
    else:
        conn.commit()
        print("Дело успешно удалено из ежедневника!")

    conn.close()

def mark_task_done():
    task_id = input("Введите ID выполненного дела: ")
    conn = sqlite3.connect("tasks.db")
    c = conn.cursor()
    c.execute("UPDATE tasks SET text = text || ' +DONE+' WHERE id=?", (task_id,))
    if c.rowcount == 0:
        print("Дело с таким ID не найдено.")
    else:
        conn.commit()
        print("Дело успешно отмечено как выполненное!")

    conn.close()

def update_task(task_id):
    new_text = input("Введите новый текст для задачи: ")
    conn = sqlite3.connect("tasks.db")
    c = conn.cursor()
    c.execute("UPDATE tasks SET text = ? WHERE id=?", (new_text, task_id))
    if c.rowcount == 0:
        print("Дело с таким ID не найдено.")
    else:
        conn.commit()
        print("Текст задачи успешно обновлен!")

    conn.close()

def write_to_journal(task_id, user_id, date, task):
    conn = sqlite3.connect("tasks.db")
    c = conn.cursor()
    c.execute("INSERT INTO tasks (id, user_id, date, text) VALUES (?, ?, ?, ?)", (task_id, user_id, date, task))
    conn.commit()
    conn.close()

def get_user_tasks(user_id):
    conn = sqlite3.connect("tasks.db")
    c = conn.cursor()
    c.execute("SELECT id, date, text FROM tasks WHERE user_id = ?", (user_id,))
    tasks = c.fetchall()
    conn.close()
    return tasks

def logout():
    print("Выход из аккаунта...")
    return None

def exit_app():
    print("Закрытие приложения...")
    quit()

def main_menu():
    print("\nГлавное меню:")
    print("1. Войти")
    print("2. Зарегистрироваться")
    print("3. Выйти")

    choice = input("Выберите действие: ")

    if choice == "1":
        username = input("Введите имя пользователя: ")
        password = getpass.getpass("Введите пароль: ")
        user_id = authenticate_user(username, password)
        if user_id:
            while True:
                print("\nОсновное меню:")
                print("1. Добавить дело")
                print("2. Пометить дело как выполненное")
                print("3. Изменить текст задачи")
                print("4. Удалить задачу")
                print("5. Просмотреть дела")
                print("6. Выйти из аккаунта")
                print("7. Закрыть приложение")
                inner_choice = input("Выберите действие: ")
                if inner_choice == "1":
                    add_task(user_id)
                elif inner_choice == "2":
                    mark_task_done()
                elif inner_choice == "3":
                    task_id = input("Введите ID задачи, которую хотите изменить: ")
                    update_task(task_id)
                elif inner_choice == "4":
                    task_id = input("Введите ID задачи, которую хотите удалить: ")
                    delete_task(task_id)
                elif inner_choice == "5":
                    tasks = get_user_tasks(user_id)
                    if tasks:
                        print("\nВаши дела:")
                        for task in tasks:
                            print(f"ID: {task[0]}, Дата: {task[1]}, Дело: {task[2]}")
                    else:
                        print("У вас нет ни одного дела.")
                elif inner_choice == "6":
                    logout()
                    break
                elif inner_choice == "7":
                    exit_app()
                else:
                    print("Некорректный выбор. Пожалуйста, выберите снова.")
        else:
            print("Не удалось аутентифицировать пользователя.")
    elif choice == "2":
        username = input("Введите имя пользователя: ")
        password = getpass.getpass("Введите пароль: ")
        register_user(username, password)
    elif choice == "3":
        print("До свидания!")
        quit()
    else:
        print("Некорректный выбор. Пожалуйста, выберите снова.")
        main_menu()

if __name__ == "__main__":
    create_users_table()
    create_tasks_table()
    while True:
        main_menu()
