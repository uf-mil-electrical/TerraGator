import sys # for command line args
from fabric import Connection # for ssh
import pty # terminals from python thx python
import os # for getting the current dir

'''
# what am i
Script for ssh'ing into the pi and deploying code

by Joseph Handsome (who thinks you should just ssh manually but idrk)

takes all the files in the current directory and copies them over to the rover PI,
replacing them in place if they already exist

### in depth explanation

step one:
    make sure there are no staged changes on the pi
step two:
    delete old files in NewCode2025
step three:
    copy all contents of NewCode2025 on local machine to NewCode2025 on PI
step four:
    either give use an ssh into machine OR run the specified command

### issues

    scp requires the password to be input :( not 1 button solution

### first use pip

pip install fabric

### example use

`python3 main.py ls`
this one will ssh in and run ls, printing the result

python3 main.py
this one will just give you an ssh into the pi
'''

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))

# returns -1 if no command
def get_command():
    if len(sys.argv) == 1:
        return -1

    return " ".join(sys.argv[1:])

def handle_result(res):
    if res.return_code != 0:
        print("--------------------------")
        print(res.stderr)
        sys.exit(1)

# literally just checks git status output and makes sure there aren't commits that we would be deleting
def no_changes_on_pi(git_status_output):
    x1 = "Your branch is up to date" in git_status_output
    # x2 = "nothing to commit, working tree clean" # I want to include this line but not ez enough and will be pain
    return x1

if __name__ == "__main__":

    # stuff for connecting over ssh
    host = input("whats the ip:")
    username = 'reptilerover'
    password = input("whats password:")

    # scrape command if it exists
    command = get_command()
    no_command = True if command == -1 else False

    connection = Connection(host=host, user=username, connect_kwargs={"password": password})

    result = connection.run("cd ReptileRover && git status", hide=True)

    # check to make sure there are no changes on the rover
    if not no_changes_on_pi(result.stdout):
        print("Hey, there are unstaged commits on the PI right now you should probably not overwrite them!!!")
        if "no" == input("are you sure you want to continue, work will be lost (yes/no):"):
            sys.exit(1)

    result = connection.run(f"rm -rf ~/ReptileRover/NewCode2025")
    handle_result(result)

    result = connection.local(f'scp -C -r {CURRENT_DIR + "/NewCode2025"} {connection.user}@{connection.host}:{"~/ReptileRover/NewCode2025"}')

    # run the command if there is one
    if no_command:
        print("here's ur shell")
        connection.shell()
    else:
        print("---------------------------")
        print("now I will run the following:")
        print(command) 
        print("---------------------------")
        result = connection.run(command)
        handle_result(result)

    connection.close()

