/* 
 * Figure out which doors in the locker room are open
 * after flipping every one, every other one, every 3rd one, ...
 */
public class LockerRoom2 {
	public static void main(String [] args){
		try {
			int n=Integer.parseInt(args[0]);
			boolean [] lockers=new boolean[n];
			System.out.println("Before flipping");
			printLockers(lockers);
			flip(lockers);
			System.out.println("After flipping");
			printLockers(lockers);
		} catch (NumberFormatException e){
			System.out.println("That was not a number");
		}
	}
	// Square locker numbers are flipped.
	static void flip(boolean [] lockers){
		for(int i=1;i*i<lockers.length;i++){
			lockers[i*i]=true;
		}
	}

	static void printLockers(boolean [] lockers){
		for(int i=0;i<lockers.length;i++){
			System.out.println(""+i+" "+lockers[i]);
		}
	}
}
