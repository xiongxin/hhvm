<?hh

function f(): void {
  $b = true;
  if ($b) {
    $d = dict['a' => 42];
  } else {
    $d = dict['b' => true];
  }
  $d['c'] = 'apple';
}
