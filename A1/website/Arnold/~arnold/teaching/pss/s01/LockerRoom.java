/* 
 * Figure out which doors in the locker room are open
 * after flipping every one, every other one, every 3rd one, ...
 */
public class LockerRoom {
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

	static void flip(boolean [] lockers){
		for(int i=1;i<=lockers.length;i++){
			for(int j=0;j<lockers.length;j=j+i){
				lockers[j]=!lockers[j];
			}
		}
	}

	static void printLockers(boolean [] lockers){
		for(int i=0;i<lockers.length;i++){
			System.out.println(""+i+" "+lockers[i]);
		}
	}
}
