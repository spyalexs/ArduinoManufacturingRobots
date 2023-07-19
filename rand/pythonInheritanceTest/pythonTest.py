class a:
    testValue = "qwertyuiop"

    def __init__(self, line):
        print(line)
    
    def sayHi(self):
        print("Hi!")

class b(a):
    def __init__(self, line):
        print("I'm an instance of b!")

        super().__init__(line)

    def sayHello(self):
        super().sayHi()

    def printTestValue(self):
        print(self.testValue)
    

inst = b("Testing")
inst.sayHello()
inst.printTestValue()