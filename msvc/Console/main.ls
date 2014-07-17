// class A {
// 	int value;

// 	public A() {
// 		value = 100;
// 	}
// }

// class B extends A {
// 	int value;

// 	public B() {
// 		value = 200;
// 	}
// }

// class C extends B {
// 	int value;

// 	public C() {
// 		value = 300;
// 	}
// }

// A a = new A();

// String testString1 = "Hello, I'm test string!";
// String testString2 = "Zzz, Welcome back segments!";

class A <T> {
	public T getT() {
		return t;
	}
	public void setT(T t) {
		this.t = t;
	}
	T t;
}

class B {
	int value = 100;
}