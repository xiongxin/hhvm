<?hh
<<file: __EnableUnstableFeatures('enum_class_label')>>

interface IBox {}
class Box<T> implements IBox {
  public function __construct(public T $data)[] {}
}
enum class E : IBox {
   Box<string> A = new Box("world");
}

class C {
    const type T = E;
}

function f<T>(<<__ViaLabel>> HH\MemberOf<C::T, Box<T>> $elt) : T {
  return $elt->data;
}

<<__EntryPoint>>
 function main() {
    $x = "A";
    echo("Hello " . f($x) . "!\n");
}
