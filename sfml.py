#Alternate compiler designed for SFML

from os import popen
from os import system as sys
from os import path
from datetime import datetime
from rich import print

# ============ FUNCTIONS ============
def getNow():
    """
    Gets current time

    :return: tuple of info
    """
    now = datetime.now()
    return now


def backup(name):
    """
    This stores a backup of the cpp file and exe for 
    later review if desired

    """
    # 1. Ensure that backup folder exists 
    if not path.exists('./backupsfml'):
        sys('mkdir backupsfml')

    # 2. Save files to backup folder
    sys(f'cp prac.cpp ./backupsfml/{name}.cpp')
    sys(f'mv {name}-app ./backupsfml/{name}-app')


def compile():
    """
    This compiles a C++ program
    
    :return: string name of output file
    """

    # 1. Determine what you'll call output
    now = getNow()
    name = f'prac-{now.year}{now.month}{now.day}-{now.hour}{now.minute}{now.second}'
    
    # 2. Try to compile
    sys(f'g++ -c prac.cpp -o {name}')

    # 3. Determine if successful
    if not path.isfile(f'{name}'):
        print('\n[bold red underline]ERROR: Compilation has failed')
        return
    
    else:
        
        # Move on to second complilation thing
        sys(f'g++ {name} -o {name}-app -lsfml-graphics -lsfml-window -lsfml-system')
        
        # Delete original elf
        sys(f'rm ./{name}')
    
    
        sys('clear')
        backup(name)
        sys(f'chmod +x ./backupsfml/{name}-app')
        sys(f'./backupsfml/{name}-app')
        sys(f'chmod -x ./backupsfml/{name}-app')
        print()
        print(f'[bold white]This iteration has been saved to the backup as [/bold white][bold blue]{name}-app[/bold blue]')
        return


# ============ START ============
if __name__ == '__main__':
    while True:
        print('[bold white]Press [/bold white][bold blue]Enter[/bold blue] [bold white]to compile and run ...[/bold white]')
        input()
        sys('clear')
        compile()

