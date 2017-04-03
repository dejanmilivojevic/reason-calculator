open Jest;
open Expect;

type tuple = { operations: list ( string, string, Action.model, int ) };
let toTuple ({ operations }: Store.state): tuple => {
  operations: List.map
    (fun { Operation.left, right, symbol, total } => ( left, right, symbol, total ))
    operations
};

let _ =

describe "store" (fun _ => {
  test "returns initial state" (fun _ => {
    let actual = Reductive.Store.getState Store.store;
    let expected = Store.init;

    expect actual |> toEqual expected;
  });

  test "reducer handles Clear action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.default,
      ]
    };
    let actual = Store.reducer state Action.Clear;
    let expected: Store.state = { operations: [] };

    expect actual |> toEqual expected;
  });

  test "reducer handles Equals action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "1" "2" Action.Add 3,
      ]
    };
    let actual = Store.reducer state Action.Equals |> toTuple;
    let expected = {
      operations: [
        ( "3", "3", Action.Equals, 3 ),
        ( "1", "2", Action.Add, 3 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles double Equals action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "3" "3" Action.Equals 3,
        Operation.create "1" "2" Action.Add 3,
      ]
    };
    let actual = Store.reducer state Action.Equals |> toTuple;
    let expected = {
      operations: [
        ( "5", "5", Action.Equals, 5 ),
        ( "3", "2", Action.Add, 5 ),
        ( "3", "3", Action.Equals, 3 ),
        ( "1", "2", Action.Add, 3 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles Input action with no previous state" (fun _ => {
    let state: Store.state = Store.init;
    let actual = Store.reducer state (Action.Input "4") |> toTuple;
    let expected = {
      operations: [
        ( "4", "", Action.Pending, 4 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles Input action with Pending state" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "1" "" Action.Pending 1,
      ]
    };
    let actual = Store.reducer state (Action.Input "4") |> toTuple;
    let expected = {
      operations: [
        ( "14", "", Action.Pending, 14 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles Input action with non-Pending state" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "1" "" Action.Add 1,
      ]
    };
    let actual = Store.reducer state (Action.Input "4") |> toTuple;
    let expected = {
      operations: [
        ( "1", "4", Action.Add, 5 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles Input action after Equals state" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "3" "3" Action.Equals 3,
        Operation.create "1" "2" Action.Add 3,
      ]
    };
    let actual = Store.reducer state (Action.Input "4") |> toTuple;
    let expected = {
      operations: [
        ( "4", "", Action.Pending, 4 ),
        ( "3", "3", Action.Equals, 3 ),
        ( "1", "2", Action.Add, 3 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles Pending action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "1" "2" Action.Add 3,
      ]
    };
    let actual = Store.reducer state Action.Pending |> toTuple;
    let expected = {
      operations: [
        ( "1", "2", Action.Add, 3 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  /* TODO: Review behavior, might not be right */
  test "reducer handles Add action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "1" "" Action.Pending 1,
      ]
    };
    let actual = Store.reducer state Action.Add |> toTuple;
    let expected = {
      operations: [
        ( "1", "", Action.Add, 1 ),
        ( "1", "", Action.Pending, 1 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles switch from Add to Subtract action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "1" "" Action.Add 1,
        Operation.create "1" "" Action.Pending 1,
      ]
    };
    let actual = Store.reducer state Action.Subtract |> toTuple;
    let expected = {
      operations: [
        ( "1", "", Action.Subtract, 1 ),
        ( "1", "", Action.Pending, 1 ),
      ]
    };

    expect actual |> toEqual expected;
  });

  test "reducer handles Add action after Equals action" (fun _ => {
    let state: Store.state = {
      operations: [
        Operation.create "2" "2" Action.Equals 2,
        Operation.create "1" "1" Action.Add 2,
        Operation.create "1" "" Action.Pending 1,
      ]
    };
    let actual = Store.reducer state Action.Add |> toTuple;
    let expected = {
      operations: [
        ( "2", "", Action.Add, 2 ),
        ( "2", "2", Action.Equals, 2 ),
        ( "1", "1", Action.Add, 2 ),
        ( "1", "", Action.Pending, 1 ),
      ]
    };

    expect actual |> toEqual expected;
  });
});
